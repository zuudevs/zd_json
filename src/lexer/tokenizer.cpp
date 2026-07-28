/**
 * @file tokenizer.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Structural tokenizer implementation using an LUT-based single pass scan
 * @version 0.3.1
 * @date 2026-07-27
 *
 * @copyright Copyright (c) 2026
 */

#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstring>

#include "constants/common.hpp"
#include "constants/lookups/token_type_lookup.hpp"
#include "constants/swar.hpp"
#include "enums/token_type.hpp"
#include "lexer/token.hpp"
#include "lexer/tokenizer.hpp"
#include "utils/bits.hpp"

namespace zuu::lexer {

std::vector<Token>
    Tokenize(std::string_view input) noexcept {

    std::vector<Token> result;
    result.reserve(input.size() / constants::kEstimatedTokenCapacityDivisor);

    const auto* const begin = reinterpret_cast<const unsigned char*>(input.data());
    const auto* ptr = begin;
    const auto* const end = begin + input.size();

    bool in_string = false;

    while (ptr < end) {
        // Fast path: pull 8 bytes at once and check, in parallel, whether
        // any of them is "interesting" for the current state. Outside a
        // string that means a quote or one of the six structural
        // characters; inside a string it means only a quote or a
        // backslash. Whitespace, digits, letters, and other plain content
        // dominate real-world JSON, so a whole block containing none of
        // these bytes is skipped in a single step; only when a match is
        // found does scanning fall back to inspecting bytes individually.
        if (ptr + constants::kUint8Len <= end) {
            uint64_t block{};
            std::memcpy(&block, ptr, sizeof(block));

            const uint64_t interesting = in_string
                                             ? (utils::HasByte(block, constants::kSwarQuote) |
                                                utils::HasByte(block, constants::kSwarBackslash))
                                             : (utils::HasByte(block, constants::kSwarQuote) |
                                                utils::HasByte(block, constants::kSwarObjectBeg) |
                                                utils::HasByte(block, constants::kSwarObjectEnd) |
                                                utils::HasByte(block, constants::kSwarArrayBeg) |
                                                utils::HasByte(block, constants::kSwarArrayEnd) |
                                                utils::HasByte(block, constants::kSwarComma) |
                                                utils::HasByte(block, constants::kSwarColon));

            if (interesting == 0) {
                ptr += constants::kUint8Len;
                continue;
            }

            ptr += static_cast<size_t>(std::countr_zero(interesting) >> 3);
        }

        if (ptr >= end) {
            break;
        }

        // Slow path: a single byte that the fast-path scan flagged (or the
        // input tail, shorter than one SWAR block) is handled individually.
        const unsigned char character = *ptr;

        if (in_string) {
            // Inside a string, structural-looking characters (',', ':', etc.)
            // are just content, not tokens. Only an unescaped '\"' matters.
            if (character == '\\') {
                ++ptr; // skip the escaped character so `\"` doesn't close the string
                if (ptr >= end) {
                    break;
                }
            } else if (character == '\"') {
                in_string = false;
            }
            ++ptr;
            continue;
        }

        if (character == '\"') {
            in_string = true;
            ++ptr;
            continue;
        }

        const auto token_type_byte = constants::lookups::kTokenTypeLookup[character];
        if (token_type_byte != constants::kUint8Max) {
            result.push_back({static_cast<enums::TokenType>(token_type_byte),
                              static_cast<uint64_t>(ptr - begin)});
        }
        ++ptr;
    }

    return result;
}

} // namespace zuu::lexer