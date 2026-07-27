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

#include <cstddef>
#include <cstdint>

#include "constants/common.hpp"
#include "constants/lookups/json_type_lookup.hpp"
#include "enums/char_class.hpp"
#include "enums/json_type.hpp"
#include "enums/token_type.hpp"
#include "lexer/value.hpp"

namespace zuu::lexer {

ScanResult
    ScanString(std::string_view input, size_t start, size_t end) noexcept {
    bool has_escape = false;
    size_t idx = start + 1;
    while (idx < end && input[idx] != '\"') {
        if (input[idx] == '\\') {
            has_escape = true;
            ++idx;
            if (idx >= end) {
                break;
            }
        }
        ++idx;
    }
    const size_t value_end = (idx < end) ? idx + 1 : end;
    const auto len = value_end - start;
    const auto type = (!has_escape && len <= constants::kMaxShortStringLength)
                          ? enums::JsonType::Short
                          : enums::JsonType::String;
    return {type, value_end};
}

[[nodiscard]] ScanResult
    ScanNumber(std::string_view input, size_t start, size_t end) noexcept {
    bool is_float = false;
    size_t idx = start + 1;
    while (idx < end) {
        const char character = input[idx];
        const uint8_t char_class = constants::lookups::kJsonTypeLookup[static_cast<uint8_t>(character)];
        if (char_class == static_cast<uint8_t>(enums::CharClass::Numeric)) {
            ++idx;
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
    while (start < end &&
           constants::lookups::kJsonTypeLookup[static_cast<uint8_t>(input[start])] == constants::kUint8Max) {
        ++start;
    }

    if (start >= end) {
        return;
    }

    const uint8_t char_class = constants::lookups::kJsonTypeLookup[static_cast<uint8_t>(input[start])];
    const char* const begin_ptr = input.data() + start;

    if (char_class == static_cast<uint8_t>(enums::CharClass::Quote)) {
        const auto result = ScanString(input, start, end);
        values.emplace_back(result.type, begin_ptr, input.data() + result.value_end);
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

} // namespace zuu::lexer