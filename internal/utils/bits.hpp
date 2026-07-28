/**
 * @file bits.hpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Brief description
 * @version 0.1.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#pragma once

#include "constants/swar.hpp"
#include <cstdint>

namespace zuu::json::utils {

// --- SWAR byte-lane helpers (Mycroft's "find zero byte" family) ---
//
// For an 8-byte word v, each helper produces a mask with the high bit of
// lane i set iff lane i satisfies the predicate. A zero result means "none
// of the 8 bytes matched"; a non-zero result lets std::countr_zero locate
// the first matching byte in O(1) instead of testing each byte in turn.
// Shared by every SWAR-accelerated scanner (ScanString, ScanNumber,
// Tokenize) so the bit-trick lives in exactly one place.

[[nodiscard]] constexpr uint64_t
    HasZero(uint64_t v) noexcept {
    return (v - constants::kSwarOne) & ~v & constants::kSwarMsb;
}

[[nodiscard]] constexpr uint64_t
    HasByte(uint64_t v, uint64_t broadcasted) noexcept {
    return HasZero(v ^ broadcasted);
}

// Valid for broadcasted values built from n in [1, 128]; matches lanes whose
// unsigned byte value is strictly less than n.
[[nodiscard]] constexpr uint64_t
    HasLess(uint64_t v, uint64_t broadcasted_n) noexcept {
    return (v - broadcasted_n) & ~v & constants::kSwarMsb;
}

} // namespace zuu::json::utils