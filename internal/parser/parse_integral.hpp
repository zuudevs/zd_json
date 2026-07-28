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
#include "constants/lookups/power_10_lookup.hpp"
#include "constants/swar.hpp"
#include "zd_json/error.hpp"

namespace zuu::parser {

[[nodiscard]] std::expected<uint64_t, JsonErrc>
    ParseIntegral(const char* first, const char* last) noexcept;

} // namespace zuu::parser