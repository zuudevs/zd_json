/**
 * @file parse_array.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Implements ParseArray.
 * @version 0.5.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include "parser/parse_array.hpp"

#include "models/arena.hpp"
#include "models/array.hpp"
#include "parser/parse_value.hpp"

namespace zuu::parser {

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

} // namespace

std::expected<models::Array*, JsonErrc>
    ParseArray(std::string_view input,
               size_t& pos,
               size_t end,
               models::Arena& arena,
               size_t depth) noexcept {
    if (pos >= end || input[pos] != '[') {
        return std::unexpected{JsonErrc::InvalidFormat};
    }

    if (depth >= kMaxParseDepth) {
        return std::unexpected{JsonErrc::DepthLimitExceeded};
    }

    ++pos; // consume '['

    models::Array::Builder builder;

    SkipWhitespace(input, pos, end);
    if (pos >= end) {
        return std::unexpected{JsonErrc::InvalidFormat};
    }

    if (input[pos] != ']') {
        for (;;) {
            auto value = ParseValue(input, pos, end, arena, depth + 1);
            if (!value) {
                return std::unexpected{value.error()};
            }
            builder.Push(*value);

            SkipWhitespace(input, pos, end);
            if (pos >= end) {
                return std::unexpected{JsonErrc::InvalidFormat};
            }

            if (input[pos] == ',') {
                ++pos;
                SkipWhitespace(input, pos, end);
                if (pos < end && input[pos] == ']') {
                    return std::unexpected{JsonErrc::TrailingComma};
                }
                continue;
            }

            if (input[pos] == ']') {
                break;
            }

            return std::unexpected{JsonErrc::MissingComma};
        }
    }

    ++pos; // consume ']'

    models::Array* array = builder.Build(arena);
    if (array == nullptr) {
        return std::unexpected{JsonErrc::Unknown};
    }

    return array;
}

} // namespace zuu::parser
