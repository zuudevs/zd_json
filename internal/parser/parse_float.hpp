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
#include "constants/lookups/power_10_lookup.hpp"
#include "zd_json/error.hpp"

namespace zuu::parser {

[[nodiscard]] std::expected<double, JsonErrc>
    ParseFloat(const char* first, const char* last) noexcept;

} // namespace zuu::parser