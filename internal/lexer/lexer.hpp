/**
 * @file lexer.hpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Value lexer: extracts and classifies JSON literal values from the
 *        input spans left between the structural tokens produced by tokenize()
 * @version 0.3.1
 * @date 2026-07-27
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
    lex_values(std::string_view input, std::span<const models::Token> tokens) noexcept;

} // namespace zuu::lexer