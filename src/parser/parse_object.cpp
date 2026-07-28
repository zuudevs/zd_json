/**
 * @file parse_object.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Implements ParseObject.
 * @version 0.5.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include "parser/parse_object.hpp"

#include "enums/json_type.hpp"
#include "lexer/lexer.hpp"
#include "lexer/scan_result.hpp"
#include "models/arena.hpp"
#include "models/object.hpp"
#include "parser/parse_string.hpp"
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

// Decodes the JSON string spanning [pos, scan.value_end) -- the zero-copy
// short-string path or the full escape-decoding path, depending on what
// the lexer detected -- and copies the result into `arena` so it outlives
// `input`. Shared shape with ParseValue's string handling, specialized
// here for object member keys.
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

std::expected<models::Object*, JsonErrc>
    ParseObject(std::string_view input,
                size_t& pos,
                size_t end,
                models::Arena& arena,
                size_t depth) noexcept {
    if (pos >= end || input[pos] != '{') {
        return std::unexpected{JsonErrc::InvalidFormat};
    }

    if (depth >= kMaxParseDepth) {
        return std::unexpected{JsonErrc::DepthLimitExceeded};
    }

    ++pos; // consume '{'

    models::Object::Builder builder;

    SkipWhitespace(input, pos, end);
    if (pos >= end) {
        return std::unexpected{JsonErrc::InvalidFormat};
    }

    if (input[pos] != '}') {
        for (;;) {
            SkipWhitespace(input, pos, end);
            if (pos >= end) {
                return std::unexpected{JsonErrc::InvalidFormat};
            }

            if (input[pos] == '\'') {
                return std::unexpected{JsonErrc::SingleQuotedString};
            }
            if (input[pos] != '"') {
                return std::unexpected{JsonErrc::UnquotedKey};
            }

            const auto key_scan = lexer::ScanString(input, pos, end);
            if (key_scan.error != JsonErrc::None) {
                return std::unexpected{key_scan.error};
            }

            auto key = DecodeArenaString(input, pos, key_scan, arena);
            if (!key) {
                return std::unexpected{key.error()};
            }
            pos = key_scan.value_end;

            SkipWhitespace(input, pos, end);
            if (pos >= end || input[pos] != ':') {
                return std::unexpected{JsonErrc::InvalidFormat};
            }
            ++pos; // consume ':'

            auto value = ParseValue(input, pos, end, arena, depth + 1);
            if (!value) {
                return std::unexpected{value.error()};
            }

            builder.Insert(*key, *value);

            SkipWhitespace(input, pos, end);
            if (pos >= end) {
                return std::unexpected{JsonErrc::InvalidFormat};
            }

            if (input[pos] == ',') {
                ++pos;
                SkipWhitespace(input, pos, end);
                if (pos < end && input[pos] == '}') {
                    return std::unexpected{JsonErrc::TrailingComma};
                }
                continue;
            }

            if (input[pos] == '}') {
                break;
            }

            return std::unexpected{JsonErrc::MissingComma};
        }
    }

    ++pos; // consume '}'

    models::Object* object = builder.Build(arena);
    if (object == nullptr) {
        return std::unexpected{JsonErrc::Unknown};
    }

    return object;
}

} // namespace zuu::parser
