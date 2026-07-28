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

namespace zuu::json::constants {

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

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
constexpr uint32_t kNullWord = 0x6c6c756e;
constexpr uint32_t kTrueWord = 0x65757274;
constexpr uint32_t kFalsWord = 0x736c6166;
#else
constexpr uint32_t kNullWord = 0x6e756c6c;
constexpr uint32_t kTrueWord = 0x74727565;
constexpr uint32_t kFalsWord = 0x66616c73;
#endif

} // namespace zuu::json::constants