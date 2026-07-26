/**
 * @file token_type.hpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Brief description
 * @version 0.3.0
 * @date 2026-07-26
 *
 * @copyright Copyright (c) 2026
 */

#pragma once

#include <cstdint>

namespace zuu::enums {

enum class TokenType : uint8_t { ObjectBeg, ObjectEnd, ArrayBeg, ArrayEnd, Comma, Colon };

} // namespace zuu::enums