/**
 * @file parse_literal.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Brief description
 * @version 0.1.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include "parser/parse_literal.hpp"

#include <cstdint>

#include "constants/common.hpp"

namespace zuu::parser {

std::expected<int8_t, JsonErrc>
    ParseLiteral(const char* first, const char* last) noexcept {
    auto current = first;
    const auto rem = last - first;

    if (rem >= 4) [[likely]] {
        uint32_t val{};
        std::memcpy(&val, current, 4);

        switch (val) {
            case constants::kNullWord:
                return int8_t{-1};
            case constants::kTrueWord:
                return int8_t{1};
            case constants::kFalsWord: {
                if (rem >= 5 && current[4] == 'e') {
                    return int8_t{0};
                }
            }
            default:
                break;
        }
    }
    return std::unexpected{JsonErrc::InvalidValue};
}

} // namespace zuu::parser