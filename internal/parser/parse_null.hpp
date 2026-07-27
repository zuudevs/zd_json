/**
 * @file parse_null.hpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Declares/defines ParseNull for the `null` JSON literal.
 * @version 0.3.1
 * @date 2026-07-27
 *
 * @copyright Copyright (c) 2026
 */

#pragma once

#include <cstddef>
#include <expected>

#include "zd_json/json_error.hpp"

namespace zuu::parser {

/**
 * @brief Parses a JSON `null` literal.
 *
 * The range [first, last) must contain exactly the literal `null`; no
 * surrounding whitespace is allowed.
 *
 * @param first Pointer to the first character of the literal.
 * @param last Exclusive pointer to one past the last character.
 * @return `nullptr` on success, or JsonErrc::InvalidNullLiteral on
 *         malformed input.
 */
[[nodiscard]] inline constexpr std::expected<std::nullptr_t, JsonErrc>
    ParseNull(const char* first, const char* last) noexcept {
    const auto len = static_cast<size_t>(last - first);

    if (len == 4 && first[0] == 'n' && first[1] == 'u' && first[2] == 'l' && first[3] == 'l') {
        return nullptr;
    }

    return std::unexpected{JsonErrc::InvalidNullLiteral};
}

} // namespace zuu::parser
