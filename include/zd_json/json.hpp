/**
 * @file json.hpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief The single public entry point for zd_json: pulls in the DOM
 *        types (Document, Value, Array, Object), the top-level Parse()
 *        function, and the Serialize()/SerializeCompact()/
 *        SerializePretty() writers.
 * @version 0.2.0
 * @date 2026-07-29
 *
 * @copyright Copyright (c) 2026
 */

#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>

#include "models/arena.hpp"
#include "models/array.hpp"
#include "models/document.hpp"
#include "models/object.hpp"
#include "models/value.hpp"
#include "serializer/serialize_options.hpp"
#include "serializer/serializer.hpp"
#include "zd_json/error.hpp"
#include "zd_json/result.hpp"
#include "zd_json/version.hpp"

/**
 * @brief The public zd_json API.
 *
 * Everything a consumer of the library needs -- the DOM node types, the
 * parser entry point, and the serializer entry points -- is reachable
 * from this single namespace after including <zd_json/json.hpp>. The
 * zuu::json::jsonparser and zuu::json::jsonserializer::detail-style implementation namespaces
 * (and everything under internal/) remain private to the library itself;
 * they are not part of the supported API surface, even though this header
 * (and the internal/ include path it depends on) is exported so the
 * library can stay header-light without duplicating type definitions.
 */
namespace zuu::json {

/// The DOM node type for a single parsed JSON value. See models::Value.
using Value = models::Value;

/// An immutable, arena-owned sequence of JSON values. See models::Array.
using Array = models::Array;

/// An immutable, arena-owned collection of JSON object members. See
/// models::Object.
using Object = models::Object;

/// The owning root of a parsed JSON DOM tree. See models::Document.
using Document = models::Document;

/// The concrete kind of value a Value node holds. See enums::JsonType.
using JsonType = enums::JsonType;

/// Formatting knobs for Serialize(). See serializer::SerializeOptions.
using SerializeOptions = serializer::SerializeOptions;

/**
 * @brief Options controlling how Parse() builds its Document.
 */
struct ParseOptions {
    /**
     * @brief Minimum size, in bytes, of the Document's first Arena
     *        block. A value of 0 falls back to
     *        models::Arena::kDefaultBlockSize.
     */
    size_t initial_block_size = models::Arena::kDefaultBlockSize;
};

/**
 * @brief Parses @p json as a single, complete JSON document.
 *
 * The entire input must be one JSON value, optionally surrounded by
 * whitespace: unlike ParseValue() (which only consumes as much of its
 * input as one value needs), Parse() fails with JsonErrc::InvalidFormat
 * if anything other than trailing whitespace remains once that value has
 * been read. Every Array, Object, and String reachable from the returned
 * Document's root() is allocated out of that Document's own Arena, so
 * the parsed tree stays valid independently of @p json for as long as
 * the Document is kept alive.
 *
 * @param json The JSON text to parse. Not required to outlive the
 *        returned Document -- all of its contents are copied into the
 *        Document's Arena.
 * @param options Tuning knobs for the Document that gets built.
 * @return The parsed Document, or the first error encountered.
 */
[[nodiscard]] Result<Document>
    Parse(std::string_view json, const ParseOptions& options = {}) noexcept;

/**
 * @brief Renders @p value -- and, recursively, everything it contains --
 *        as JSON text. See serializer::Serialize().
 */
[[nodiscard]] inline std::string
    Serialize(const Value& value, const SerializeOptions& options = {}) noexcept {
    return serializer::Serialize(value, options);
}

/**
 * @brief Serialize() with the default compact options (no inter-token
 *        whitespace). See serializer::SerializeCompact().
 */
[[nodiscard]] inline std::string
    SerializeCompact(const Value& value) noexcept {
    return serializer::SerializeCompact(value);
}

/**
 * @brief Serialize() with pretty-printing enabled. See
 *        serializer::SerializePretty().
 */
[[nodiscard]] inline std::string
    SerializePretty(const Value& value, uint8_t indent_size = 2) noexcept {
    return serializer::SerializePretty(value, indent_size);
}

} // namespace zuu::json