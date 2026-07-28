/**
 * @file power_10_lookup.hpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Brief description
 * @version 0.3.0
 * @date 2026-07-27
 *
 * @copyright Copyright (c) 2026
 */

#pragma once

#include <cstdint>

namespace zuu::json::constants::lookups {

constexpr uint64_t kIntegralPower10Lookup[]{1, 10, 100, 1000, 10000, 100000, 1000000, 10000000};
constexpr double kFloatPower10NegLookup[]{1e0,   1e-1,  1e-2,  1e-3,  1e-4,  1e-5,  1e-6,  1e-7,
                                          1e-8,  1e-9,  1e-10, 1e-11, 1e-12, 1e-13, 1e-14, 1e-15,
                                          1e-16, 1e-17, 1e-18, 1e-19, 1e-20, 1e-21, 1e-22};
constexpr double kFloatPower10PosLookup[]{1e0,  1e1,  1e2,  1e3,  1e4,  1e5,  1e6,  1e7,
                                          1e8,  1e9,  1e10, 1e11, 1e12, 1e13, 1e14, 1e15,
                                          1e16, 1e17, 1e18, 1e19, 1e20, 1e21, 1e22};

} // namespace zuu::json::constants::lookups