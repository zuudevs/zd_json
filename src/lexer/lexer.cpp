/**
 * @file lexer.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Implements the JSON value lexer.
 * @version 0.3.1
 * @date 2026-07-27
 *
 * @copyright Copyright (c) 2026
 */

#include "lexer/lexer.hpp"

#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstring>

#include "constants/common.hpp"
#include "constants/lookups/json_type_lookup.hpp"
#include "constants/swar.hpp"
#include "enums/char_class.hpp"
#include "enums/json_type.hpp"
#include "enums/token_type.hpp"
#include "lexer/value.hpp"
#include "utils/bits.hpp"
#include "zd_json/error.hpp"

namespace zuu::json::lexer {

namespace {

using utils::HasByte;
using utils::HasLess;

// Validates a single UTF-8 sequence starting at *p (p < end is guaranteed by
// the caller). Always advances `advance` by at least 1 so the caller makes
// forward progress even on malformed input.
[[nodiscard]] JsonErrc
    ValidateUtf8Sequence(const unsigned char* p,
                         const unsigned char* end,
                         size_t& advance) noexcept {
    const auto is_cont = [](unsigned char b) noexcept { return (b & 0xC0) == 0x80; };
    const auto remaining = static_cast<size_t>(end - p);
    const unsigned char lead = p[0];

    if ((lead & 0xE0) == 0xC0) { // 2-byte sequence
        if (remaining < 2) {
            advance = remaining;
            return JsonErrc::InvalidUnicode;
        }
        advance = 2;
        if (lead < 0xC2 || !is_cont(p[1])) { // 0xC0/0xC1 are always overlong
            return JsonErrc::InvalidUnicode;
        }
        return JsonErrc::None;
    }

    if ((lead & 0xF0) == 0xE0) { // 3-byte sequence
        if (remaining < 3) {
            advance = remaining;
            return JsonErrc::InvalidUnicode;
        }
        advance = 3;
        if (!is_cont(p[1]) || !is_cont(p[2])) {
            return JsonErrc::InvalidUnicode;
        }
        if (lead == 0xE0 && p[1] < 0xA0) { // overlong encoding
            return JsonErrc::InvalidUnicode;
        }
        if (lead == 0xED && p[1] >= 0xA0) { // U+D800..U+DFFF encoded directly
            return JsonErrc::InvalidSurrogate;
        }
        return JsonErrc::None;
    }

    if ((lead & 0xF8) == 0xF0) { // 4-byte sequence
        if (remaining < 4) {
            advance = remaining;
            return JsonErrc::InvalidUnicode;
        }
        advance = 4;
        if (!is_cont(p[1]) || !is_cont(p[2]) || !is_cont(p[3])) {
            return JsonErrc::InvalidUnicode;
        }
        if (lead == 0xF0 && p[1] < 0x90) { // overlong encoding
            return JsonErrc::InvalidUnicode;
        }
        if (lead > 0xF4 || (lead == 0xF4 && p[1] > 0x8F)) { // beyond U+10FFFF
            return JsonErrc::InvalidUnicode;
        }
        return JsonErrc::None;
    }

    // Stray continuation byte (0x80-0xBF) or invalid lead (0xF8-0xFF).
    advance = 1;
    return JsonErrc::InvalidUnicode;
}

} // namespace

ScanResult
    ScanString(std::string_view input, size_t start, size_t end) noexcept {
    bool has_escape = false;
    JsonErrc error = JsonErrc::None;
    size_t idx = start + 1;

    const auto* const base = reinterpret_cast<const unsigned char*>(input.data());

    while (idx < end) {
        // Fast path: pull 8 bytes at once and check, in parallel, whether
        // any of them is a quote, backslash, control character, or
        // non-ASCII byte. If none are, the whole block is plain string
        // content and can be skipped in one step.
        if (idx + constants::kUint8Len <= end) {
            uint64_t block{};
            std::memcpy(&block, base + idx, sizeof(block));

            const uint64_t interesting =
                HasByte(block, constants::kSwarQuote) | HasByte(block, constants::kSwarBackslash) |
                HasLess(block, constants::kSwarSpace) | (block & constants::kSwarMsb);

            if (interesting == 0) {
                idx += constants::kUint8Len;
                continue;
            }

            idx += static_cast<size_t>(std::countr_zero(interesting) >> 3);
        }

        if (idx >= end) {
            break;
        }

        const unsigned char character = base[idx];

        if (character == '\"') {
            break;
        }

        if (character == '\\') {
            has_escape = true;
            ++idx;
            if (idx >= end) {
                break;
            }
            ++idx; // skip the escaped character; ParseString validates its meaning
            continue;
        }

        if (character < 0x20) {
            if (error == JsonErrc::None) {
                error = JsonErrc::UnescapedCharacter;
            }
            ++idx;
            continue;
        }

        if (character >= 0x80) {
            size_t advance = 1;
            const JsonErrc seq_error = ValidateUtf8Sequence(base + idx, base + end, advance);
            if (seq_error != JsonErrc::None && error == JsonErrc::None) {
                error = seq_error;
            }
            idx += advance;
            continue;
        }

        ++idx; // plain ASCII byte sharing a block with something interesting
    }

    const bool closed = idx < end;
    const size_t value_end = closed ? idx + 1 : end;
    if (!closed && error == JsonErrc::None) {
        error = JsonErrc::InvalidFormat; // never found the closing quote
    }

    const auto len = value_end - start;
    const auto type = (!has_escape && len <= constants::kMaxShortStringLength)
                          ? enums::JsonType::Short
                          : enums::JsonType::String;
    return {type, value_end, error};
}

[[nodiscard]] ScanResult
    ScanNumber(std::string_view input, size_t start, size_t end) noexcept {
    bool is_float = false;
    size_t idx = start + 1;

    const auto* const base = reinterpret_cast<const unsigned char*>(input.data());

    while (idx < end) {
        // Fast path: pull 8 bytes at once and check, in parallel, whether
        // all of them are ASCII digits ('0'-'9'). Runs of digits dominate a
        // JSON number's bytes, so a block that is entirely digits can be
        // skipped in one step instead of being classified byte by byte.
        if (idx + constants::kUint8Len <= end) {
            uint64_t block{};
            std::memcpy(&block, base + idx, sizeof(block));

            const uint64_t val = block - constants::kSwarZero;
            const uint64_t non_digits =
                ((val + constants::kSwarDigitBias) | val) & constants::kSwarMsb;

            if (non_digits == 0) {
                idx += constants::kUint8Len;
                continue;
            }

            idx += static_cast<size_t>(std::countr_zero(non_digits) >> 3);
        }

        if (idx >= end) {
            break;
        }

        // Slow path: a single non-digit character -- '.', sign, 'e'/'E', or
        // the number's true end -- decides how (or whether) scanning
        // continues.
        const char character = static_cast<char>(base[idx]);
        const uint8_t char_class =
            constants::lookups::kJsonTypeLookup[static_cast<uint8_t>(character)];
        if (char_class == static_cast<uint8_t>(enums::CharClass::Numeric)) {
            ++idx; // digit adjacent to a block boundary, not part of the SWAR run above
        } else if (char_class == static_cast<uint8_t>(enums::CharClass::Dot)) {
            is_float = true;
            ++idx;
        } else if (char_class == static_cast<uint8_t>(enums::CharClass::Sign) || character == 'e' ||
                   character == 'E') {
            is_float = is_float || (character == 'e' || character == 'E');
            ++idx;
        } else {
            break;
        }
    }
    return {is_float ? enums::JsonType::Float : enums::JsonType::Integer, idx};
}

[[nodiscard]] size_t
    ScanAlpha(std::string_view input, size_t start, size_t end) noexcept {
    size_t idx = start + 1;
    while (idx < end && constants::lookups::kJsonTypeLookup[static_cast<uint8_t>(input[idx])] ==
                            static_cast<uint8_t>(enums::CharClass::Alpha)) {
        ++idx;
    }
    return idx;
}

void
    ProcessRange(std::string_view input,
                 size_t start,
                 size_t end,
                 std::vector<Value>& values) noexcept {
    if (start >= end) {
        return;
    }

    // Skip leading whitespace only (LUT != 255). Each scan_* helper below
    // finds its own value's end by scanning forward, so no backward trim
    // over the same bytes is needed afterwards.
    while (start < end && constants::lookups::kJsonTypeLookup[static_cast<uint8_t>(input[start])] ==
                              constants::kUint8Max) {
        ++start;
    }

    if (start >= end) {
        return;
    }

    const uint8_t char_class =
        constants::lookups::kJsonTypeLookup[static_cast<uint8_t>(input[start])];
    const char* const begin_ptr = input.data() + start;

    if (char_class == static_cast<uint8_t>(enums::CharClass::Quote)) {
        const auto result = ScanString(input, start, end);
        values.emplace_back(result.type, begin_ptr, input.data() + result.value_end, result.error);
    } else if (char_class == static_cast<uint8_t>(enums::CharClass::Numeric) ||
               char_class == static_cast<uint8_t>(enums::CharClass::Sign)) {
        const auto result = ScanNumber(input, start, end);
        values.emplace_back(result.type, begin_ptr, input.data() + result.value_end);
    } else if (char_class == static_cast<uint8_t>(enums::CharClass::Alpha)) {
        const char first_char = input[start];
        const size_t value_end = ScanAlpha(input, start, end);
        if (first_char == 't' || first_char == 'f') {
            values.emplace_back(enums::JsonType::Bool, begin_ptr, input.data() + value_end);
        } else if (first_char == 'n') {
            values.emplace_back(enums::JsonType::Null, begin_ptr, input.data() + value_end);
        }
    }
}

std::vector<Value>
    LexValues(std::string_view input, std::span<const Token> tokens) noexcept {

    std::vector<Value> values;
    values.reserve(tokens.size() + 4);

    size_t last_pos = 0;

    for (const auto& tok : tokens) {
        if (tok.pos > last_pos) {
            ProcessRange(input, last_pos, tok.pos, values);
        }

        const auto token_type = static_cast<enums::TokenType>(tok.type);
        if (token_type == enums::TokenType::ObjectBeg) {
            values.emplace_back(
                enums::JsonType::Object, input.data() + tok.pos, input.data() + tok.pos + 1);
        } else if (token_type == enums::TokenType::ArrayBeg) {
            values.emplace_back(
                enums::JsonType::Array, input.data() + tok.pos, input.data() + tok.pos + 1);
        }

        last_pos = tok.pos + 1;
    }

    if (last_pos < input.size()) {
        ProcessRange(input, last_pos, input.size(), values);
    }

    return values;
}

} // namespace zuu::json::lexer