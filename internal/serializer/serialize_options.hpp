/**
 * @file serialize_options.hpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Declares SerializeOptions, the formatting knobs shared by every
 *        serializer entry point.
 * @version 0.6.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#pragma once

#include <cstdint>

namespace zuu::json::serializer {

/**
 * @brief Formatting knobs shared by every Write/Serialize entry point.
 *
 * `pretty` selects between the compact writer (no inter-token whitespace
 * at all, the smallest possible output) and the pretty writer (one
 * member/element per line, indented by `indent_size` spaces per nesting
 * level, with a space after each member's `:`). `indent_size` is ignored
 * when `pretty` is false.
 */
struct SerializeOptions {
    bool pretty = false;
    uint8_t indent_size = 2;
};

} // namespace zuu::json::serializer