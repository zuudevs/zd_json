/**
 * @file char_class.hpp
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

enum class CharClass : uint8_t {
	Alp,
	Dot,
	Num,
	Sig,
	Quo,
};

} // namespace zuu::enums