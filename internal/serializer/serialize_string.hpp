/**
 * @file serialize_string.hpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Declares WriteEscapedString for formatting JSON string literals.
 * @version 0.6.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#pragma once

#include <string>
#include <string_view>

namespace zuu::serializer {

/**
 * @brief Appends the quoted, escaped JSON string literal for @p value to
 *        @p out.
 *
 * Escapes the two characters JSON requires an escape for ('"' and '\\'),
 * uses the short two-character escapes for the common control characters
 * (\\b \\f \\n \\r \\t), and falls back to a \\u00XX escape for every other
 * byte below 0x20. Every other byte -- including the unescaped '/' JSON
 * permits but doesn't require escaping, and multi-byte UTF-8 sequences --
 * is copied through unchanged; this mirrors zuu::parser::ParseString,
 * which likewise leaves raw, already-valid UTF-8 bytes untouched.
 *
 * @param out Buffer to append to; the surrounding quotation marks are
 *        written as part of this call.
 * @param value Decoded string content (not yet quoted or escaped).
 */
void
    WriteEscapedString(std::string& out, std::string_view value) noexcept;

} // namespace zuu::serializer