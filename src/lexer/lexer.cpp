/**
 * @file lexer.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Lexer implementation for JSON token slice conversion
 * @version 0.3.0
 * @date 2026-07-26
 *
 * @copyright Copyright (c) 2026
 */

#include "lexer/lexer.hpp"

#include <cstddef>
#include <cstdint>

#include "constants/common.hpp"
#include "enums/char_class.hpp"
#include "enums/json_type.hpp"
#include "enums/token_type.hpp"
#include "lookups/json_type_lookup.hpp"
#include "models/value.hpp"

namespace zuu::lexer {

namespace {

[[nodiscard]] enums::JsonType
    classify_string(std::string_view input, size_t start, size_t end) noexcept {
    bool has_escape = false;
    for (size_t idx = start; idx < end; ++idx) {
        if (input[idx] == '\\') {
            has_escape = true;
            break;
        }
    }
    const auto len = end - start;
    return (!has_escape && len <= constants::kMaxShortStringLength) ? enums::JsonType::Short
                                                                    : enums::JsonType::String;
}

[[nodiscard]] enums::JsonType
    classify_number(std::string_view input, size_t start, size_t end) noexcept {
    bool is_float = false;
    for (size_t idx = start; idx < end; ++idx) {
        const char character = input[idx];
        if (character == '.' || character == 'e' || character == 'E') {
            is_float = true;
            break;
        }
    }
    return is_float ? enums::JsonType::Float : enums::JsonType::Integer;
}

void
    process_range(std::string_view input,
                  size_t start,
                  size_t end,
                  std::vector<models::Value>& values) noexcept {
    if (start >= end) {
        return;
    }

    // Skip non-value characters (whitespace, structural tokens, etc.) using LUT != 255
    while (start < end &&
           lookups::kJsonTypeLookup[static_cast<uint8_t>(input[start])] == constants::kUint8Max) {
        ++start;
    }
    while (end > start &&
           lookups::kJsonTypeLookup[static_cast<uint8_t>(input[end - 1])] == constants::kUint8Max) {
        --end;
    }

    if (start >= end) {
        return;
    }

    const uint8_t char_class = lookups::kJsonTypeLookup[static_cast<uint8_t>(input[start])];
    const char* const begin_ptr = input.data() + start;
    const char* const end_ptr = input.data() + end;

    if (char_class == static_cast<uint8_t>(enums::CharClass::Quo)) {
        const auto type = classify_string(input, start, end);
        values.emplace_back(type, begin_ptr, end_ptr);
    } else if (char_class == static_cast<uint8_t>(enums::CharClass::Num) ||
               char_class == static_cast<uint8_t>(enums::CharClass::Sig)) {
        const auto type = classify_number(input, start, end);
        values.emplace_back(type, begin_ptr, end_ptr);
    } else if (char_class == static_cast<uint8_t>(enums::CharClass::Alp)) {
        const char first_char = input[start];
        if (first_char == 't' || first_char == 'f') {
            values.emplace_back(enums::JsonType::Bool, begin_ptr, end_ptr);
        } else if (first_char == 'n') {
            values.emplace_back(enums::JsonType::Null, begin_ptr, end_ptr);
        }
    }
}

} // namespace

std::vector<models::Value>
    lexer(std::string_view input, std::span<const models::Token> tokens) noexcept {

    std::vector<models::Value> values;
    values.reserve(tokens.size() + 4);

    size_t last_pos = 0;

    for (const auto& tok : tokens) {
        if (tok.pos > last_pos) {
            process_range(input, last_pos, tok.pos, values);
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
        process_range(input, last_pos, input.size(), values);
    }

    return values;
}

} // namespace zuu::lexer