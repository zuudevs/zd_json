/**
 * @file lexer.hpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Declares functions for extracting JSON values from structural token ranges.
 * @version 0.3.1
 * @date 2026-07-27
 *
 * @copyright Copyright (c) 2026
 */

#pragma once

#include <span>
#include <string_view>
#include <vector>

#include "token.hpp"
#include "value.hpp"
#include "scan_result.hpp"

namespace zuu::lexer {

/**
 * @brief Scans a JSON string value starting at the opening quotation mark.
 *
 * Tracks escape sequences and locates the matching closing quotation mark
 * in a single forward pass.
 *
 * @param input Input JSON text.
 * @param start Index of the opening quotation mark.
 * @param end Exclusive upper bound of the scan range.
 * @return The detected string type and its exclusive end index.
 */
[[nodiscard]] ScanResult
    ScanString(std::string_view input, size_t start, size_t end) noexcept;

/**
 * @brief Scans a JSON number starting at its first digit or sign.
 *
 * Stops at the first character that is not part of the number,
 * determining both its numeric type and exclusive end index.
 *
 * @param input Input JSON text.
 * @param start Index of the first digit or sign.
 * @param end Exclusive upper bound of the scan range.
 * @return The detected number type and its exclusive end index.
 */
[[nodiscard]] ScanResult
    ScanNumber(std::string_view input, size_t start, size_t end) noexcept;

/**
 * @brief Scans an alphabetic JSON literal.
 *
 * Scans the literals `true`, `false`, or `null` until the first
 * non-alphabetic character.
 *
 * @param input Input JSON text.
 * @param start Index of the first alphabetic character.
 * @param end Exclusive upper bound of the scan range.
 * @return Exclusive end index of the scanned literal.
 */
[[nodiscard]] size_t
    ScanAlpha(std::string_view input, size_t start, size_t end) noexcept;

/**
 * @brief Extracts JSON values from a contiguous input range.
 *
 * Skips leading whitespace, identifies the value type,
 * scans the value, and appends the resulting Value objects.
 *
 * @param input Input JSON text.
 * @param start Inclusive start index.
 * @param end Exclusive end index.
 * @param values Destination container.
 */
void
    ProcessRange(std::string_view input,
                  size_t start,
                  size_t end,
                  std::vector<Value>& values) noexcept;

/**
 * @brief Converts structural tokens into JSON values.
 *
 * Processes the gaps between structural tokens, classifies primitive
 * values, and emits object and array values directly from their opening
 * structural tokens.
 *
 * @param input Input JSON text.
 * @param tokens Structural tokens produced by tokenize().
 * @return Extracted JSON values.
 */
[[nodiscard]] std::vector<Value>
    LexValues(std::string_view input, std::span<const Token> tokens) noexcept;

} // namespace zuu::lexer