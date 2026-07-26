/**
 * @file tokenizer.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Brief description
 * @version 0.3.0
 * @date 2026-07-26
 *
 * @copyright Copyright (c) 2026
 */

#include <cstddef>
#include <cstdint>

#include "constants/common.hpp"
#include "enums/token_type.hpp"
#include "lexer/tokenizer.hpp"
#include "lookups/token_type_lookup.hpp"
#include "models/token.hpp"

namespace zuu::lexer {

std::vector<models::Token>
    tokenize(std::string_view input) noexcept {

    std::vector<models::Token> result;
    result.reserve(input.size() / constants::kEstimatedTokenCapacityDivisor);

    const auto* const begin = input.data();
    const auto* ptr = input.data();
    const auto* const end = ptr + input.size();

    while (ptr != nullptr && ptr < end) {
        auto tok = lookups::kTokenTypeLookup[static_cast<uint8_t>(*ptr)];
        if (tok != constants::kUint8Max) {
            result.push_back(
                {static_cast<enums::TokenType>(tok), static_cast<uint64_t>(ptr - begin)});
        }
        ++ptr;
    }

    return result;
}

} // namespace zuu::lexer