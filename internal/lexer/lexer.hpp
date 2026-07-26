/**
 * @file lexer.hpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Brief description
 * @version 0.3.0
 * @date 2026-07-26
 *
 * @copyright Copyright (c) 2026
 */

#pragma once

#include <span>
#include <string_view>
#include <vector>

#include "models/token.hpp"
#include "models/value.hpp"

namespace zuu::lexer {

[[nodiscard]] std::vector<models::Value>
    lexer(std::string_view input, std::span<const models::Token> tokens) noexcept;

} // namespace zuu::lexer