/**
 * @file parse_bool.hpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Declares/defines ParseBool for the `true`/`false` JSON literals.
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
 * @brief Parses a JSON boolean literal (`true` or `false`).
 *
 * The range [first, last) must contain exactly one of the two literals;
 * no surrounding whitespace is allowed. This mirrors what
 * zuu::lexer::ScanAlpha hands off: the exact span of an alphabetic token.
 *
 * @param first Pointer to the first character of the literal.
 * @param last Exclusive pointer to one past the last character.
 * @return The parsed boolean, or JsonErrc::InvalidBooleanLiteral on
 *         malformed input.
 */
[[nodiscard]] std::expected<bool, JsonErrc>
    ParseBool(const char* first, const char* last) noexcept;

} // namespace zuu::parser
