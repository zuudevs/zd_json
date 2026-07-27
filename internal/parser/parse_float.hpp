/**
 * @file parse_float.hpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Brief description
 * @version 0.3.0
 * @date 2026-07-27
 *
 * @copyright Copyright (c) 2026
 */

#pragma once

#include <charconv>
#include <cstdint>
#include <expected>

#include "constants/common.hpp"
#include "lookups/power_10_lookup.hpp"
#include "zd_json/json_error.hpp"

namespace zuu::parser {

[[nodiscard]] inline constexpr std::expected<double, JsonErrc>
    ParseFloat(const char* first, const char* last) noexcept {
    if (first == last) {
        return std::unexpected{JsonErrc::InvalidFormat};
    }

    const char* ptr = first;
    bool is_negative = (*ptr == constants::kCharNegative);
    ptr += is_negative;

    uint64_t mantissa{};
    int32_t decimal_shift{};
    int32_t digits{};

    while (ptr != last && *ptr != '.' && *ptr != 'e' && *ptr != 'E') {
        mantissa = mantissa * constants::kDigit + (*ptr - constants::kCharZero);
        digits++;
        ++ptr;
    }

    if (ptr != last && *ptr == '.') {
        ++ptr;
        while (ptr != last && *ptr != 'e' && *ptr != 'E') {
            mantissa = mantissa * constants::kDigit + (*ptr - constants::kCharZero);
            decimal_shift--;
            digits++;
            ++ptr;
        }
    }

    if (ptr != last && (*ptr == 'e' || *ptr == 'E')) {
        ++ptr;
        bool exp_negative = (*ptr == constants::kCharNegative);
        ptr += (exp_negative || *ptr == constants::kCharPositive);
        int32_t exponent{};
        while (ptr != last) {
            exponent = exponent * constants::kDigit + (*ptr - constants::kCharZero);
            ++ptr;
        }
        decimal_shift += exp_negative ? -exponent : exponent;
    }

    if (digits <= 19 && mantissa <= 9007199254740991ULL && decimal_shift >= -22 &&
        decimal_shift <= 22) {
        auto result = static_cast<double>(mantissa);

        if (decimal_shift > 0) {
            result *= lookups::kFloatPower10PosLookup[decimal_shift];
        } else if (decimal_shift > 0) {
            result *= lookups::kFloatPower10NegLookup[-decimal_shift];
        }

        return is_negative ? -result : result;
    }

    auto result = static_cast<double>(0);
    auto [end_ptr, ec] = std::from_chars(first, last, result);

    if (ec == std::errc()) {
        return result;
    }

    return std::unexpected{JsonErrc::InvalidFormat};
}

} // namespace zuu::parser