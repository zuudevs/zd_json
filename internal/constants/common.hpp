/**
 * @file common.hpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Brief description
 * @version 0.3.0
 * @date 2026-07-26
 *
 * @copyright Copyright (c) 2026
 */

#pragma once

#include <cstddef>
#include <cstdint>

namespace zuu::constants {

constexpr uint8_t kUint8Max = 255;
constexpr uint8_t kEstimatedTokenCapacityDivisor = 8;
constexpr std::size_t kMaxShortStringLength = 16;

} // namespace zuu::constants