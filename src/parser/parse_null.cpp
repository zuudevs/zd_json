/**
 * @file parse_null.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Brief description
 * @version 0.3.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include "parser/parse_null.hpp"

namespace zuu::json::parser {

std::expected<std::nullptr_t, JsonErrc>
    ParseNull(const char* first, const char* last) noexcept {
    const auto len = static_cast<size_t>(last - first);

    if (len == 4 && first[0] == 'n' && first[1] == 'u' && first[2] == 'l' && first[3] == 'l') {
        return nullptr;
    }

    return std::unexpected{JsonErrc::InvalidNullLiteral};
}

} // namespace zuu::json::parser