/**
 * @file serialize_string.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Implements WriteEscapedString.
 * @version 0.6.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include "serializer/serialize_string.hpp"

#include <array>

namespace zuu::serializer {

namespace {

constexpr std::array<char, 16> kHexDigits = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

// Appends a `\u00XX` escape for a control character below 0x20 that has no
// short (\b \f \n \r \t) escape of its own.
void
    AppendUnicodeEscape(std::string& out, unsigned char byte) noexcept {
    out.append("\\u00");
    out.push_back(kHexDigits[(byte >> 4) & 0x0FU]);
    out.push_back(kHexDigits[byte & 0x0FU]);
}

} // namespace

void
    WriteEscapedString(std::string& out, std::string_view value) noexcept {
    out.push_back('\"');

    for (const char c : value) {
        const auto character = static_cast<unsigned char>(c);
        switch (character) {
            case '\"':
                out.append("\\\"");
                break;
            case '\\':
                out.append("\\\\");
                break;
            case '\b':
                out.append("\\b");
                break;
            case '\f':
                out.append("\\f");
                break;
            case '\n':
                out.append("\\n");
                break;
            case '\r':
                out.append("\\r");
                break;
            case '\t':
                out.append("\\t");
                break;
            default:
                if (character < 0x20) {
                    AppendUnicodeEscape(out, character);
                } else {
                    out.push_back(static_cast<char>(character));
                }
                break;
        }
    }

    out.push_back('\"');
}

} // namespace zuu::serializer