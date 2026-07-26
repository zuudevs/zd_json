/**
 * @file tokenizer.hpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Brief description
 * @version 0.3.0
 * @date 2026-07-26
 *
 * @copyright Copyright (c) 2026
 */

#pragma once

#include <string_view>
#include <vector>

#include "models/token.hpp"

namespace zuu::lexer {

[[nodiscard]]
std::vector<models::Token>
    tokenize(std::string_view input) noexcept;

} // namespace zuu::lexer