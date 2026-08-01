/**
 * @file parse_string.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Brief description
 * @version 0.3.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include "parser/parse_string.hpp"

#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstring>

#include "constants/common.hpp"
#include "constants/swar.hpp"
#include "utils/bits.hpp"

namespace zuu::json::parser {

namespace detail {

[[nodiscard]] constexpr bool
    IsHexDigit(char c) noexcept {
    return (c >= constants::kCharZero && c <= '9') || (c >= 'a' && c <= 'f') ||
           (c >= 'A' && c <= 'F');
}

[[nodiscard]] constexpr int
    HexValue(char c) noexcept {
    if (c >= constants::kCharZero && c <= '9') {
        return c - constants::kCharZero;
    }
    if (c >= 'a' && c <= 'f') {
        return c - 'a' + constants::kDigit;
    }
    return c - 'A' + constants::kDigit;
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

std::expected<std::string_view, JsonErrc>
    ParseShortString(const char* first, const char* last) noexcept {
    if (last - first < 2 || *first != '\"' || *(last - 1) != '\"') {
        return std::unexpected{JsonErrc::InvalidFormat};
    }

    return std::string_view{first + 1, static_cast<size_t>(last - first - 2)};
}

std::expected<std::string, JsonErrc>
    ParseString(const char* first, const char* last) noexcept {
    if (last - first < 2 || *first != '\"' || *(last - 1) != '\"') {
        return std::unexpected{JsonErrc::InvalidFormat};
    }

    const char* ptr = first + 1;
    const char* const content_end = last - 1;

    std::string out;
    out.reserve(static_cast<size_t>(content_end - ptr));

    while (ptr < content_end) {
        // ---------------------------------------------------------------
        // SWAR fast path: consume 8 plain ASCII bytes in a single step.
        //
        // A byte is "interesting" if it is a backslash (needs escape
        // handling), a control character (< 0x20, illegal unescaped), or
        // a non-ASCII byte (>= 0x80, begins a multibyte UTF-8 sequence).
        // When none of the 8 bytes are interesting, the whole block is
        // unambiguous ASCII content and can be bulk-appended at once.
        // When at least one is interesting, countr_zero locates the first
        // such byte in O(1) and only the clean prefix is bulk-appended
        // before falling through to the byte-by-byte slow path.
        // ---------------------------------------------------------------
        if (content_end - ptr >= static_cast<ptrdiff_t>(constants::kUint8Len)) {
            uint64_t block{};
            std::memcpy(&block, ptr, sizeof(block));

            // Detect backslash, control chars (< 0x20), and non-ASCII (>= 0x80).
            const uint64_t interesting =
                utils::HasByte(block, constants::kSwarBackslash) |
                utils::HasLess(block, constants::kSwarSpace) |
                (block & constants::kSwarMsb);

            if (interesting == 0) {
                // All 8 bytes are plain ASCII with no special characters.
                out.append(ptr, constants::kUint8Len);
                ptr += constants::kUint8Len;
                continue;
            }

            // Advance past the clean prefix bytes before the first
            // interesting byte, then fall through to the slow path below.
            const size_t skip = static_cast<size_t>(std::countr_zero(interesting) >> 3);
            out.append(ptr, skip);
            ptr += skip;
        }

        if (ptr >= content_end) {
            break;
        }

        // ---------------------------------------------------------------
        // Slow path: process a single byte (or escape sequence).
        // ---------------------------------------------------------------
        const unsigned char character = static_cast<unsigned char>(*ptr);

        if (character == '\\') {
            ++ptr;
            if (ptr >= content_end) {
                return std::unexpected{JsonErrc::InvalidValue};
            }

            const char escape = *ptr;
            switch (escape) {
                case '\"':
                    out.push_back('\"');
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

        // Plain byte (ASCII 0x20-0x7E, or a non-ASCII UTF-8 continuation/lead
        // byte that sits adjacent to a block boundary). The lexer has already
        // validated that all non-ASCII sequences are well-formed, so we just
        // copy the byte through without re-checking.
        out.push_back(static_cast<char>(character));
        ++ptr;
    }

    return out;
}

} // namespace zuu::json::parser