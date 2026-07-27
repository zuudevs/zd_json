/**
 * @file value.hpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Brief description
 * @version 0.3.0
 * @date 2026-07-26
 *
 * @copyright Copyright (c) 2026
 */

#pragma once

#include <string_view>

#include "enums/json_type.hpp"
#include "zd_json/json_error.hpp"

namespace zuu::lexer {

/**
 * @brief
 *
 */
struct Value {
    enums::JsonType type;
    std::string_view value;

    /**
     * @brief First validation error found while scanning this value, if any.
     *
     * Currently only populated for string values (see ScanString); other
     * value kinds default to JsonErrc::None.
     */
    JsonErrc error = JsonErrc::None;

    constexpr Value(enums::JsonType type,
                    const char* begin,
                    const char* end,
                    JsonErrc error = JsonErrc::None) noexcept
        : type(type)
        , value(begin, end)
        , error(error) {}
};

} // namespace zuu::lexer