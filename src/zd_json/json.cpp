/**
 * @file json.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Implements zuu::json::jsonParse, the public top-level parsing entry point.
 * @version 0.2.0
 * @date 2026-07-29
 *
 * @copyright Copyright (c) 2026
 */

#include "zd_json/json.hpp"

#include "parser/parse_value.hpp"

namespace zuu::json {

namespace {

[[nodiscard]] constexpr bool
    IsJsonWhitespace(char character) noexcept {
    return character == ' ' || character == '\t' || character == '\n' || character == '\r';
}

// Advances `pos` past any trailing whitespace and reports whether
// anything other than whitespace remains in [pos, end).
[[nodiscard]] bool
    HasTrailingContent(std::string_view json, size_t& pos, size_t end) noexcept {
    while (pos < end && IsJsonWhitespace(json[pos])) {
        ++pos;
    }
    return pos != end;
}

} // namespace

Result<Document>
    Parse(std::string_view json, const ParseOptions& options) noexcept {
    Document document(options.initial_block_size);

    size_t pos = 0;
    const size_t end = json.size();

    auto value = parser::ParseValue(json, pos, end, document.arena(), 0);
    if (!value) {
        return Err(value.error());
    }

    if (HasTrailingContent(json, pos, end)) {
        return Err(JsonErrc::InvalidFormat);
    }

    document.set_root(*value);
    return document;
}

} // namespace zuu::json