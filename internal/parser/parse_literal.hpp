/**
 * @file parse_literal.hpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Brief description
 * @version 0.3.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#pragma once

#include <cstdint>
#include <expected>

#include "zd_json/error.hpp"

namespace zuu::parser {

[[nodiscard]] std::expected<int8_t, JsonErrc>
    ParseLiteral(const char* first, const char* last) noexcept;

} // namespace zuu::parser