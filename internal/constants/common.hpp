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

#include <cstdint>

namespace zuu::constants {

#ifdef __AVX2__
constexpr uint8_t kBlockSize = 16;
#else
constexpr uint8_t kBlockSize = 8;
#endif

constexpr uint8_t kUint8Max = 255;
constexpr uint8_t kUint8Len = 8;
constexpr uint8_t kUint16Len = 16;
constexpr uint8_t kUint32Len = 32;

constexpr uint8_t kEstimatedTokenCapacityDivisor = 8;
constexpr uint8_t kMaxShortStringLength = 16;
constexpr uint8_t kDigit = 10;

constexpr char kCharPositive = '+';
constexpr char kCharNegative = '-';
constexpr char kCharZero = '0';

} // namespace zuu::constants