/**
 * @file tokenizer.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Structural tokenizer implementation using an LUT-based single pass scan
 * @version 0.3.1
 * @date 2026-07-27
 *
 * @copyright Copyright (c) 2026
 */

#include <cstddef>
#include <cstdint>

#include "constants/common.hpp"
#include "constants/lookups/token_type_lookup.hpp"
#include "enums/token_type.hpp"
#include "lexer/token.hpp"
#include "lexer/tokenizer.hpp"

namespace zuu::lexer {

std::vector<Token>
    Tokenize(std::string_view input) noexcept {

    std::vector<Token> result;
    result.reserve(input.size() / constants::kEstimatedTokenCapacityDivisor);

    const auto* const begin = input.data();
    const auto* ptr = input.data();
    const auto* const end = ptr + input.size();

    bool in_string = false;

    while (ptr < end) {
        const char character = *ptr;

        if (in_string) {
            // Inside a string, structural-looking characters (',', ':', etc.)
            // are just content, not tokens. Only an unescaped '"' matters.
            if (character == '\\') {
                ++ptr; // skip the escaped character so `\"` doesn't close the string
                if (ptr >= end) {
                    break;
                }
            } else if (character == '"') {
                in_string = false;
            }
            ++ptr;
            continue;
        }

        if (character == '"') {
            in_string = true;
            ++ptr;
            continue;
        }

        const auto token_type_byte =
            constants::lookups::kTokenTypeLookup[static_cast<uint8_t>(character)];
        if (token_type_byte != constants::kUint8Max) {
            result.push_back({static_cast<enums::TokenType>(token_type_byte),
                              static_cast<uint64_t>(ptr - begin)});
        }
        ++ptr;
    }

    return result;
}

} // namespace zuu::lexer