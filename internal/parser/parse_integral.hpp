/**
 * @file parse_integral.hpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Brief description
 * @version 0.3.0
 * @date 2026-07-27
 *
 * @copyright Copyright (c) 2026
 */

#pragma once

#include <bit>
#include <cstdint>
#include <cstring>
#include <expected>

#include "constants/common.hpp"
#include "constants/swar.hpp"
#include "lookups/power_10_lookup.hpp"
#include "zd_json/json_error.hpp"

namespace zuu::parser {

[[nodiscard]] inline constexpr std::expected<uint64_t, JsonErrc>
    ParseIntegral(const char* first, const char* last) noexcept {
    if (first == last) {
        return std::unexpected{JsonErrc::InvalidFormat};
    }

    uint64_t value{};
    bool is_negative{};
    bool has_digits{};

    if (first < last && *first == constants::kCharNegative) {
        is_negative = true;
        ++first;
    }

    while (first + constants::kUint8Len <= last) {
        uint64_t block{};
        std::memcpy(&block, first, sizeof(block));

        uint64_t val = block - constants::kSwar8Zero;
        uint64_t non_digits = ((val + constants::kSwar8DigitBias) | val) & constants::kSwar8Msb;

        if (non_digits == 0) [[likely]] {
            has_digits = true;
            uint64_t d = val & 0x0F0F0F0F0F0F0F0FULL;
            d = (d * constants::kDigit) + (d >> constants::kUint8Len);
            d = (d & 0x00FF00FF00FF00FFULL) * 100 +
                ((d >> constants::kUint16Len) & 0x00FF00FF00FF00FFULL);
            d = (d & 0x0000FFFF0000FFFFULL) * 10000 +
                ((d >> constants::kUint32Len) & 0x0000FFFF0000FFFFULL);
            value = value * 100000000ULL + static_cast<uint32_t>(d);
            first += constants::kUint8Len;
        } else {
            uint32_t valid_len = std::countr_zero(non_digits) >> 3;
            if (valid_len == 0) {
                break;
            }
            has_digits = true;
            uint64_t shift_bits = (constants::kUint8Len - valid_len) * constants::kUint8Len;
            val <<= shift_bits;

            uint64_t d = val & 0x0F0F0F0F0F0F0F0FULL;
            d = (d * constants::kDigit) + (d >> constants::kUint8Len);
            d = (d & 0x00FF00FF00FF00FFULL) * 100 +
                ((d >> constants::kUint16Len) & 0x00FF00FF00FF00FFULL);
            d = (d & 0x0000FFFF0000FFFFULL) * 10000 +
                ((d >> constants::kUint32Len) & 0x0000FFFF0000FFFFULL);

            value = value * lookups::kIntegralPower10Lookup[valid_len] + static_cast<uint32_t>(d);
            first += valid_len;
            break;
        }
    }

    while (first < last &&
           static_cast<uint8_t>(*first - constants::kCharZero) < constants::kDigit) {
        has_digits = true;
        value = value * constants::kDigit + (*first - constants::kCharZero);
        ++first;
    }

    if (!has_digits || first != last) {
        return std::unexpected{JsonErrc::InvalidFormat};
    }

    return {(is_negative ? -static_cast<uint64_t>(value) : static_cast<uint64_t>(value))};
}

} // namespace zuu::parser