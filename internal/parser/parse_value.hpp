/**
 * @file parse_value.hpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Declares ParseValue, the recursive-descent dispatcher shared by
 *        ParseArray and ParseObject.
 * @version 0.5.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#pragma once

#include <cstddef>
#include <expected>
#include <string_view>

#include "models/value.hpp"
#include "zd_json/error.hpp"

namespace zuu::models {
class Arena;
} // namespace zuu::models

namespace zuu::parser {

/**
 * @brief Maximum container nesting depth permitted while parsing.
 *
 * Enforced by ParseArray and ParseObject so that deeply/maliciously nested
 * input fails fast with JsonErrc::DepthLimitExceeded instead of exhausting
 * the call stack via unbounded recursion.
 */
inline constexpr size_t kMaxParseDepth = 512;

/**
 * @brief Parses a single JSON value starting at @p pos.
 *
 * Skips leading whitespace, classifies the value from its first
 * significant character, and dispatches to the matching scanner/decoder --
 * recursing into ParseArray or ParseObject when the value is a container.
 * Any string, Array, or Object produced is allocated out of @p arena, so it
 * remains valid for as long as @p arena does, independent of the lifetime
 * of @p input.
 *
 * @param input Input JSON text.
 * @param pos In/out cursor. Must point at the value's first significant
 *        character (or at leading whitespace before it) on entry; advanced
 *        to one past the parsed value on success. Left unspecified on
 *        failure.
 * @param end Exclusive upper bound of the parse range.
 * @param arena Arena backing any Array/Object/string allocations produced.
 * @param depth Current container nesting depth; the top-level caller
 *        passes 0. Propagated (and incremented) into any nested
 *        ParseArray/ParseObject call so kMaxParseDepth is enforced across
 *        the whole document, not just one container's direct children.
 * @return The parsed value, or the first error encountered -- including
 *         JsonErrc::EmptyValue if only whitespace (or nothing) remains in
 *         [pos, end).
 */
[[nodiscard]] std::expected<models::Value, JsonErrc>
    ParseValue(std::string_view input,
               size_t& pos,
               size_t end,
               models::Arena& arena,
               size_t depth) noexcept;

} // namespace zuu::parser
