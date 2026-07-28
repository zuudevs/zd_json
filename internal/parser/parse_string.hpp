/**
 * @file parse_string.hpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Declares/defines ParseShortString and ParseString for JSON string
 *        values.
 * @version 0.3.1
 * @date 2026-07-27
 *
 * @copyright Copyright (c) 2026
 */

#pragma once

#include <cstddef>
#include <cstdint>
#include <expected>
#include <string>
#include <string_view>

#include "zd_json/json_error.hpp"

namespace zuu::parser {

namespace detail {

[[nodiscard]] constexpr bool
    IsHexDigit(char c) noexcept {
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

[[nodiscard]] constexpr int
    HexValue(char c) noexcept {
    if (c >= '0' && c <= '9') {
        return c - '0';
    }
    if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    }
    return c - 'A' + 10;
}

// Parses exactly 4 hex digits starting at ptr, advancing ptr past them on
// success. Used to decode the XXXX in a `\uXXXX` escape.
[[nodiscard]] constexpr std::expected<uint16_t, JsonErrc>
    ParseHex4(const char*& ptr, const char* last) noexcept {
    if (last - ptr < 4) {
        return std::unexpected{JsonErrc::InvalidValue};
    }

    uint16_t value{};
    for (int i = 0; i < 4; ++i) {
        const char c = ptr[i];
        if (!IsHexDigit(c)) {
            return std::unexpected{JsonErrc::InvalidValue};
        }
        value = static_cast<uint16_t>((value << 4) | HexValue(c));
    }

    ptr += 4;
    return value;
}

// Encodes a Unicode code point (already combined from a surrogate pair, if
// applicable) as UTF-8 and appends it to out.
constexpr void
    AppendUtf8(std::string& out, uint32_t codepoint) {
    if (codepoint <= 0x7F) {
        out.push_back(static_cast<char>(codepoint));
    } else if (codepoint <= 0x7FF) {
        out.push_back(static_cast<char>(0xC0 | (codepoint >> 6)));
        out.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
    } else if (codepoint <= 0xFFFF) {
        out.push_back(static_cast<char>(0xE0 | (codepoint >> 12)));
        out.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
        out.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
    } else {
        out.push_back(static_cast<char>(0xF0 | (codepoint >> 18)));
        out.push_back(static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F)));
        out.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
        out.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
    }
}

} // namespace detail

/**
 * @brief Zero-copy fast path for JsonType::Short string values.
 *
 * The lexer only ever classifies a string as Short when it contains no
 * escape sequences, so this simply strips the surrounding quotation marks
 * and returns a view into the original buffer -- no allocation, no decode.
 *
 * @param first Pointer to the opening quotation mark.
 * @param last Exclusive pointer to one past the closing quotation mark.
 * @return A view of the string's content, or JsonErrc::InvalidFormat if
 *         [first, last) isn't a properly quoted span.
 */
[[nodiscard]] inline constexpr std::expected<std::string_view, JsonErrc>
    ParseShortString(const char* first, const char* last) noexcept {
    if (last - first < 2 || *first != '"' || *(last - 1) != '"') {
        return std::unexpected{JsonErrc::InvalidFormat};
    }

    return std::string_view{first + 1, static_cast<size_t>(last - first - 2)};
}

/**
 * @brief Decodes a JSON string value into an owned std::string.
 *
 * Handles the standard escape sequences (`\"`, `\\`, `\/`, `\b`, `\f`,
 * `\n`, `\r`, `\t`) as well as `\uXXXX` escapes, including combining UTF-16
 * surrogate pairs into a single code point before re-encoding as UTF-8.
 * Raw (non-escaped) bytes are copied through as-is: zuu::lexer::ScanString
 * already validates that they form well-formed UTF-8 and contain no
 * unescaped control characters, so this function does not repeat that
 * check on the caller's behalf; the [first, last) span should come from a
 * lexer::Value whose `error` field is JsonErrc::None.
 *
 * @param first Pointer to the opening quotation mark.
 * @param last Exclusive pointer to one past the closing quotation mark.
 * @return The decoded string, or an error describing the first problem
 *         found (JsonErrc::UnescapedCharacter, JsonErrc::InvalidSurrogate,
 *         JsonErrc::InvalidValue, or JsonErrc::InvalidFormat).
 */
[[nodiscard]] inline std::expected<std::string, JsonErrc>
    ParseString(const char* first, const char* last) {
    if (last - first < 2 || *first != '\"' || *(last - 1) != '"') {
        return std::unexpected{JsonErrc::InvalidFormat};
    }

    const char* ptr = first + 1;
    const char* const content_end = last - 1;

    std::string out;
    out.reserve(static_cast<size_t>(content_end - ptr));

    while (ptr < content_end) {
        const unsigned char character = static_cast<unsigned char>(*ptr);

        if (character == '\\') {
            ++ptr;
            if (ptr >= content_end) {
                return std::unexpected{JsonErrc::InvalidValue};
            }

            const char escape = *ptr;
            switch (escape) {
                case '\"':
                    out.push_back('"');
                    ++ptr;
                    break;
                case '\\':
                    out.push_back('\\');
                    ++ptr;
                    break;
                case '/':
                    out.push_back('/');
                    ++ptr;
                    break;
                case 'b':
                    out.push_back('\b');
                    ++ptr;
                    break;
                case 'f':
                    out.push_back('\f');
                    ++ptr;
                    break;
                case 'n':
                    out.push_back('\n');
                    ++ptr;
                    break;
                case 'r':
                    out.push_back('\r');
                    ++ptr;
                    break;
                case 't':
                    out.push_back('\t');
                    ++ptr;
                    break;
                case 'u': {
                    ++ptr;
                    const auto high = detail::ParseHex4(ptr, content_end);
                    if (!high) {
                        return std::unexpected{high.error()};
                    }

                    uint32_t codepoint = *high;

                    if (codepoint >= 0xD800 && codepoint <= 0xDBFF) {
                        // High surrogate: must be immediately followed by a
                        // low surrogate escape to form a valid code point.
                        if (content_end - ptr < 6 || ptr[0] != '\\' || ptr[1] != 'u') {
                            return std::unexpected{JsonErrc::InvalidSurrogate};
                        }
                        ptr += 2;
                        const auto low = detail::ParseHex4(ptr, content_end);
                        if (!low) {
                            return std::unexpected{low.error()};
                        }
                        if (*low < 0xDC00 || *low > 0xDFFF) {
                            return std::unexpected{JsonErrc::InvalidSurrogate};
                        }
                        codepoint = 0x10000 + ((codepoint - 0xD800) << 10) + (*low - 0xDC00);
                    } else if (codepoint >= 0xDC00 && codepoint <= 0xDFFF) {
                        // Lone low surrogate: never valid on its own.
                        return std::unexpected{JsonErrc::InvalidSurrogate};
                    }

                    detail::AppendUtf8(out, codepoint);
                    break;
                }
                default:
                    return std::unexpected{JsonErrc::InvalidValue};
            }
            continue;
        }

        if (character < 0x20) {
            return std::unexpected{JsonErrc::UnescapedCharacter};
        }

        out.push_back(static_cast<char>(character));
        ++ptr;
    }

    return out;
}

} // namespace zuu::parser
