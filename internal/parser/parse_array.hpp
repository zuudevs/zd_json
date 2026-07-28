/**
 * @file parse_array.hpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Declares ParseArray for decoding a JSON array into a models::Array.
 * @version 0.5.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#pragma once

#include <cstddef>
#include <expected>
#include <string_view>

#include "zd_json/error.hpp"

namespace zuu::models {
class Arena;
class Array;
} // namespace zuu::models

namespace zuu::parser {

/**
 * @brief Parses a JSON array value into an arena-owned models::Array.
 *
 * Parses zero or more comma-separated elements (each itself parsed via
 * ParseValue, so elements may be any JSON value, including nested arrays
 * and objects) between a leading `[` and a matching `]`. Whitespace around
 * elements, commas, and the brackets is skipped. A trailing comma before
 * `]` is rejected, as is a missing comma between elements.
 *
 * @param input Input JSON text.
 * @param pos In/out cursor. Must point at the opening `[` on entry (no
 *        leading whitespace skipped here); advanced to one past the
 *        matching closing `]` on success. Left unspecified on failure.
 * @param end Exclusive upper bound of the parse range.
 * @param arena Arena that will own the resulting Array, along with any
 *        nested Array/Object/string values it contains.
 * @param depth Current container nesting depth; the top-level caller
 *        passes 0. Checked against kMaxParseDepth before descending.
 * @return Pointer to the parsed Array, or the first error encountered:
 *         JsonErrc::InvalidFormat if @p pos doesn't point at `[` or the
 *         array is unterminated, JsonErrc::DepthLimitExceeded if nesting
 *         exceeds kMaxParseDepth, JsonErrc::TrailingComma, or
 *         JsonErrc::MissingComma -- plus whatever error an element's
 *         ParseValue call reports.
 */
[[nodiscard]] std::expected<models::Array*, JsonErrc>
    ParseArray(std::string_view input,
               size_t& pos,
               size_t end,
               models::Arena& arena,
               size_t depth) noexcept;

} // namespace zuu::parser
