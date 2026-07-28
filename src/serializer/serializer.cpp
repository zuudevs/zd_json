/**
 * @file serializer.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Implements Serialize, SerializeCompact, and SerializePretty.
 * @version 0.6.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include "serializer/serializer.hpp"

#include <cstddef>

#include "enums/json_type.hpp"
#include "models/array.hpp"
#include "models/object.hpp"
#include "serializer/serialize_number.hpp"
#include "serializer/serialize_string.hpp"

namespace zuu::serializer {

namespace {

// Bundles the output buffer and formatting options threaded through the
// recursive Write* helpers below, so each only takes the arguments that
// actually change as it descends -- the node being written and its
// nesting depth.
struct Writer {
    std::string& out;
    const SerializeOptions& options;

    // In pretty mode, starts a new line and indents it to `depth` levels;
    // a no-op in compact mode.
    void
        WriteNewlineAndIndent(size_t depth) const noexcept {
        if (!options.pretty) {
            return;
        }
        out.push_back('\n');
        out.append(static_cast<size_t>(options.indent_size) * depth, ' ');
    }

    void
        WriteArray(const models::Array& array, size_t depth) const noexcept {
        out.push_back('[');

        if (!array.empty()) {
            bool first = true;
            for (const models::Value& element : array) {
                if (!first) {
                    out.push_back(',');
                }
                first = false;

                WriteNewlineAndIndent(depth + 1);
                WriteValue(element, depth + 1);
            }
            WriteNewlineAndIndent(depth);
        }

        out.push_back(']');
    }

    void
        WriteObject(const models::Object& object, size_t depth) const noexcept {
        out.push_back('{');

        if (!object.empty()) {
            bool first = true;
            for (const auto& member : object) {
                if (!first) {
                    out.push_back(',');
                }
                first = false;

                WriteNewlineAndIndent(depth + 1);
                WriteEscapedString(out, member.key);
                out.push_back(':');
                if (options.pretty) {
                    out.push_back(' ');
                }
                WriteValue(member.value, depth + 1);
            }
            WriteNewlineAndIndent(depth);
        }

        out.push_back('}');
    }

    void
        WriteValue(const models::Value& value, size_t depth) const noexcept {
        switch (value.type()) {
            case enums::JsonType::Null:
                out.append("null");
                break;
            case enums::JsonType::Bool:
                out.append(*value.as_bool() ? "true" : "false");
                break;
            case enums::JsonType::Integer:
                WriteInteger(out, *value.as_integer());
                break;
            case enums::JsonType::Float:
                WriteFloat(out, *value.as_float());
                break;
            case enums::JsonType::String:
                WriteEscapedString(out, *value.as_string());
                break;
            case enums::JsonType::Array:
                WriteArray(**value.as_array(), depth);
                break;
            case enums::JsonType::Object:
                WriteObject(**value.as_object(), depth);
                break;
            case enums::JsonType::Short:
                // JsonType::Short is a lexer-only classification for
                // zero-copy, escape-free string tokens; it's always decoded
                // into a plain JsonType::String before reaching a
                // models::Value, so this case is unreachable in practice.
                out.append("null");
                break;
        }
    }
};

} // namespace

std::string
    Serialize(const models::Value& value, const SerializeOptions& options) noexcept {
    std::string out;
    const Writer writer{out, options};
    writer.WriteValue(value, 0);
    return out;
}

std::string
    SerializeCompact(const models::Value& value) noexcept {
    return Serialize(value, SerializeOptions{});
}

std::string
    SerializePretty(const models::Value& value, uint8_t indent_size) noexcept {
    return Serialize(value, SerializeOptions{.pretty = true, .indent_size = indent_size});
}

} // namespace zuu::serializer