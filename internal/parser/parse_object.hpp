/**
 * @file parse_object.hpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Declares ParseObject for decoding a JSON object into a
 *        models::Object.
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

namespace zuu::json::models {
class Arena;
class Object;
} // namespace zuu::json::models

namespace zuu::json::parser {

/**
 * @brief Parses a JSON object value into an arena-owned models::Object.
 *
 * Parses zero or more comma-separated `"key": value` members between a
 * leading `{` and a matching `}`. Each key must be a double-quoted JSON
 * string; each member's value is parsed via ParseValue, so it may be any
 * JSON value, including nested arrays and objects. Whitespace around
 * members, colons, commas, and the braces is skipped. Duplicate keys are
 * permitted and preserved in insertion order, matching models::Object's
 * own semantics. A trailing comma before `}` is rejected, as is a missing
 * comma between members.
 *
 * @param input Input JSON text.
 * @param pos In/out cursor. Must point at the opening `{` on entry (no
 *        leading whitespace skipped here); advanced to one past the
 *        matching closing `}` on success. Left unspecified on failure.
 * @param end Exclusive upper bound of the parse range.
 * @param arena Arena that will own the resulting Object, along with any
 *        nested Array/Object/string values (including decoded keys) it
 *        contains.
 * @param depth Current container nesting depth; the top-level caller
 *        passes 0. Checked against kMaxParseDepth before descending.
 * @return Pointer to the parsed Object, or the first error encountered:
 *         JsonErrc::InvalidFormat if @p pos doesn't point at `{`, the
 *         object is unterminated, or a member is missing its `:`
 *         separator; JsonErrc::DepthLimitExceeded if nesting exceeds
 *         kMaxParseDepth; JsonErrc::UnquotedKey or
 *         JsonErrc::SingleQuotedString for a malformed key;
 *         JsonErrc::TrailingComma or JsonErrc::MissingComma for member
 *         separators -- plus whatever error a key's or value's decoding
 *         reports.
 */
[[nodiscard]] std::expected<models::Object*, JsonErrc>
    ParseObject(std::string_view input,
                size_t& pos,
                size_t end,
                models::Arena& arena,
                size_t depth) noexcept;

} // namespace zuu::json::parser
