/**
 * @file token.hpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Brief description
 * @version 0.3.0
 * @date 2026-07-26
 *
 * @copyright Copyright (c) 2026
 */

#pragma once

#include <concepts>
#include <cstdint>
#include <type_traits>

#include "enums/token_type.hpp"

namespace zuu::lexer {

/**
 * @brief 
 * 
 */
class Token {
  public:
    uint64_t type : 8;
    uint64_t pos : 55;

    constexpr Token(enums::TokenType type, uint64_t pos) noexcept
        : type(static_cast<uint64_t>(type))
        , pos(pos) {}
};

static_assert(std::is_scoped_enum_v<enums::TokenType>, "TokenType must be a scoped enum.");

static_assert(std::same_as<std::underlying_type_t<enums::TokenType>, std::uint8_t>,
              "TokenType must use std::uint8_t as its underlying type.");

static_assert(sizeof(Token) == sizeof(std::uint64_t), "Token must occupy exactly 8 bytes.");

static_assert(alignof(Token) == alignof(std::uint64_t), "Unexpected Token alignment.");

static_assert(std::is_trivially_copyable_v<Token>, "Token must be trivially copyable.");

static_assert(std::is_standard_layout_v<Token>, "Token must have standard layout.");

} // namespace zuu::lexer