/**
 * @file parse_bool.hpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Brief description
 * @version 0.3.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include "parser/parse_bool.hpp"

namespace zuu::json::parser {

std::expected<bool, JsonErrc>
    ParseBool(const char* first, const char* last) noexcept {
    const auto len = static_cast<size_t>(last - first);

    if (len == 4 && first[0] == 't' && first[1] == 'r' && first[2] == 'u' && first[3] == 'e') {
        return true;
    }

    if (len == 5 && first[0] == 'f' && first[1] == 'a' && first[2] == 'l' && first[3] == 's' &&
        first[4] == 'e') {
        return false;
    }

    return std::unexpected{JsonErrc::InvalidBooleanLiteral};
}

} // namespace zuu::json::parser