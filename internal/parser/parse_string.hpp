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

#include <expected>
#include <string>
#include <string_view>

#include "zd_json/json_error.hpp"

namespace zuu::parser {

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
[[nodiscard]] std::expected<std::string_view, JsonErrc>
    ParseShortString(const char* first, const char* last) noexcept;

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
[[nodiscard]] std::expected<std::string, JsonErrc>
    ParseString(const char* first, const char* last) noexcept;

} // namespace zuu::parser
