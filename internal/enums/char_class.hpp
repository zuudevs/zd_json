/**
 * @file char_class.hpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Character classification categories used by the value lexer
 * @version 0.3.1
 * @date 2026-07-27
 *
 * @copyright Copyright (c) 2026
 */

#pragma once

#include <cstdint>

namespace zuu::json::enums {

enum class CharClass : uint8_t {
    Alpha,
    Dot,
    Numeric,
    Sign,
    Quote,
};

} // namespace zuu::json::enums