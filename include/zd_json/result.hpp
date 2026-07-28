/**
 * @file result.hpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Declares Result<T>, the library-wide alias for a fallible value.
 * @version 0.2.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#pragma once

#include <expected>

#include "zd_json/error.hpp"

namespace zuu::json {

/**
 * @brief The result of any fallible zd_json operation.
 *
 * Result<T> is a thin alias over std::expected<T, JsonErrc>: on success it
 * holds a `T`, and on failure it holds the JsonErrc describing what went
 * wrong. It is the return type of every public entry point that can fail
 * (Parse(), Value::as_*(), Array::at(), Object::Find(), ...), so callers
 * consistently check `result.has_value()` / `bool(result)` and unwrap via
 * `*result` or `result.value()`, or inspect the failure via
 * `result.error()` and zuu::json::to_string().
 *
 * @tparam T The success payload type.
 */
template <typename T>
using Result = std::expected<T, JsonErrc>;

/**
 * @brief Convenience factory for a failed Result<T>.
 *
 * Equivalent to `std::unexpected<JsonErrc>(code)`, spelled out so callers
 * can write `return Err(JsonErrc::InvalidType);` instead of naming
 * std::unexpected explicitly.
 *
 * @param code The failure to wrap. Implicitly convertible to any
 *        Result<T> via std::expected's converting constructor.
 */
[[nodiscard]] constexpr std::unexpected<JsonErrc>
    Err(JsonErrc code) noexcept {
    return std::unexpected<JsonErrc>(code);
}

} // namespace zuu::json