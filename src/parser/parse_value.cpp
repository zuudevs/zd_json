/**
 * @file parse_value.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Implements ParseValue.
 * @version 0.5.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include "parser/parse_value.hpp"

#include <cstdint>

#include "enums/json_type.hpp"
#include "lexer/lexer.hpp"
#include "lexer/scan_result.hpp"
#include "models/arena.hpp"
#include "parser/parse_array.hpp"
#include "parser/parse_float.hpp"
#include "parser/parse_integral.hpp"
#include "parser/parse_literal.hpp"
#include "parser/parse_object.hpp"
#include "parser/parse_string.hpp"

namespace zuu::json::parser {

namespace {

[[nodiscard]] constexpr bool
    IsJsonWhitespace(char character) noexcept {
    return character == ' ' || character == '\t' || character == '\n' || character == '\r';
}

void
    SkipWhitespace(std::string_view input, size_t& pos, size_t end) noexcept {
    while (pos < end && IsJsonWhitespace(input[pos])) {
        ++pos;
    }
}

// Decodes the JSON string spanning [pos, scan.value_end) -- the zero-copy
// short-string path or the full escape-decoding path, depending on what
// the lexer detected -- and copies the result into `arena` so it outlives
// `input`.
[[nodiscard]] std::expected<std::string_view, JsonErrc>
    DecodeArenaString(std::string_view input,
                      size_t pos,
                      const lexer::ScanResult& scan,
                      models::Arena& arena) noexcept {
    const char* const first = input.data() + pos;
    const char* const last = input.data() + scan.value_end;

    if (scan.type == enums::JsonType::Short) {
        auto view = ParseShortString(first, last);
        if (!view) {
            return std::unexpected{view.error()};
        }
        return arena.CreateString(*view);
    }

    auto decoded = ParseString(first, last);
    if (!decoded) {
        return std::unexpected{decoded.error()};
    }
    return arena.CreateString(*decoded);
}

} // namespace

std::expected<models::Value, JsonErrc>
    ParseValue(std::string_view input,
               size_t& pos,
               size_t end,
               models::Arena& arena,
               size_t depth) noexcept {
    SkipWhitespace(input, pos, end);

    if (pos >= end) {
        return std::unexpected{JsonErrc::EmptyValue};
    }

    const char* const base = input.data();
    const char character = input[pos];

    if (character == '"') {
        const auto scan = lexer::ScanString(input, pos, end);
        if (scan.error != JsonErrc::None) {
            return std::unexpected{scan.error};
        }
        auto decoded = DecodeArenaString(input, pos, scan, arena);
        if (!decoded) {
            return std::unexpected{decoded.error()};
        }
        pos = scan.value_end;
        return models::Value{*decoded};
    }

    if (character == '{') {
        auto object = ParseObject(input, pos, end, arena, depth);
        if (!object) {
            return std::unexpected{object.error()};
        }
        return models::Value{*object};
    }

    if (character == '[') {
        auto array = ParseArray(input, pos, end, arena, depth);
        if (!array) {
            return std::unexpected{array.error()};
        }
        return models::Value{*array};
    }

    if (character == 't' || character == 'f' || character == 'n') {
        const size_t value_end = lexer::ScanAlpha(input, pos, end);
        const auto literal = ParseLiteral(base + pos, base + value_end);
        if (!literal) {
            return std::unexpected{literal.error()};
        }
        pos = value_end;
        if (*literal < 0) {
            return models::Value{nullptr};
        }
        return models::Value{static_cast<bool>(*literal)};
    }

    if (character == '-' || (character >= '0' && character <= '9')) {
        const auto scan = lexer::ScanNumber(input, pos, end);
        const char* const first = base + pos;
        const char* const last = base + scan.value_end;

        if (scan.type == enums::JsonType::Integer) {
            auto integer = ParseIntegral(first, last);
            if (!integer) {
                return std::unexpected{integer.error()};
            }
            pos = scan.value_end;
            return models::Value{static_cast<int64_t>(*integer)};
        }

        auto value = ParseFloat(first, last);
        if (!value) {
            return std::unexpected{value.error()};
        }
        pos = scan.value_end;
        return models::Value{*value};
    }

    return std::unexpected{JsonErrc::InvalidValue};
}

} // namespace zuu::json::parser
