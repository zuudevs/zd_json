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

#include "enums/json_type.hpp"
#include <string_view>

namespace zuu::models {

struct Value {
    enums::JsonType type;
    std::string_view value;

    constexpr Value(enums::JsonType type, const char* begin, const char* end) noexcept
        : type(type)
        , value(begin, end) {}
};

} // namespace zuu::models