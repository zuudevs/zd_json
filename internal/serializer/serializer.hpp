/**
 * @file serializer.hpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Declares Serialize, the recursive DOM-to-JSON-text writer, and its
 *        SerializeCompact/SerializePretty convenience wrappers.
 * @version 0.6.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#pragma once

#include <cstdint>
#include <string>

#include "models/value.hpp"
#include "serializer/serialize_options.hpp"

namespace zuu::serializer {

/**
 * @brief Renders @p value -- and, recursively, everything it contains --
 *        as JSON text.
 *
 * Dispatches on value.type(): scalars (Null, Bool, Integer, Float, String)
 * are written directly via WriteInteger/WriteFloat/WriteEscapedString, and
 * the Array/Object containers recurse into their elements/members,
 * honoring options.pretty for indentation and line breaks along the way.
 * Object members are written in their original insertion order (the same
 * order Object::members() reports), including any duplicate keys.
 *
 * There is no failure return here: a Value produced by this library's own
 * parser, or built up by hand from valid pieces, always has a JSON
 * representation. The only way this can misbehave is std::bad_alloc from
 * the underlying std::string, which -- consistent with the rest of this
 * library -- is left to std::terminate rather than surfaced as an error.
 *
 * @param value Root of the (sub)tree to serialize.
 * @param options Formatting knobs; defaults to compact output.
 * @return The rendered JSON text.
 */
[[nodiscard]] std::string
    Serialize(const models::Value& value, const SerializeOptions& options = {}) noexcept;

/**
 * @brief Convenience wrapper: Serialize() with the default compact
 *        options (no inter-token whitespace).
 */
[[nodiscard]] std::string
    SerializeCompact(const models::Value& value) noexcept;

/**
 * @brief Convenience wrapper: Serialize() with pretty-printing enabled.
 *
 * @param indent_size Number of spaces per nesting level.
 */
[[nodiscard]] std::string
    SerializePretty(const models::Value& value, uint8_t indent_size = 2) noexcept;

} // namespace zuu::serializer