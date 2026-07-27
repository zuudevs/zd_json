/**
 * @file swar.hpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Brief description
 * @version 0.3.0
 * @date 2026-07-27
 *
 * @copyright Copyright (c) 2026
 */

#pragma once

#include <cstdint>

namespace zuu::constants {

constexpr uint64_t kSwarZero = 0x3030303030303030;
constexpr uint64_t kSwarOne = 0x101010101010101;
constexpr uint64_t kSwarMsb = 0x8080808080808080;
constexpr uint64_t kSwarDigitBias = 0x7676767676767676;

// Byte broadcast constants used by the SWAR string scanner (ScanString) to
// find, within a single 8-byte load, the first occurrence of a quote,
// backslash, ASCII control character (< 0x20), or non-ASCII byte (>= 0x80).
constexpr uint64_t kSwarQuote = 0x2222222222222222;     // '\"' (0x22) repeated
constexpr uint64_t kSwarBackslash = 0x5C5C5C5C5C5C5C5C; // '\' (0x5C) repeated
constexpr uint64_t kSwarSpace = 0x2020202020202020; // 0x20 repeated, threshold for control chars

} // namespace zuu::constants