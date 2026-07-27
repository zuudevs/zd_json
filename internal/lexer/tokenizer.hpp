/**
 * @file tokenizer.hpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Structural tokenizer: scans the input for JSON structural
 *        characters ('{', '}', '[', ']', ',', ':') and records their positions
 * @version 0.3.1
 * @date 2026-07-27
 *
 * @copyright Copyright (c) 2026
 */

#pragma once

#include <string_view>
#include <vector>

#include "lexer/token.hpp"

namespace zuu::lexer {

[[nodiscard]]
std::vector<lexer::Token>
    Tokenize(std::string_view input) noexcept;

} // namespace zuu::lexer