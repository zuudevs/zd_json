/**
 * @file serialize_number.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Implements WriteInteger and WriteFloat.
 * @version 0.6.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include "serializer/serialize_number.hpp"

#include <array>
#include <charconv>
#include <cmath>
#include <cstddef>
#include <string_view>

namespace zuu::json::serializer {

void
    WriteInteger(std::string& out, int64_t value) noexcept {
    // 20 digits (INT64_MIN) plus a leading '-' fits in 21 bytes; round up
    // for headroom.
    std::array<char, 24> buffer{};
    const auto result = std::to_chars(buffer.data(), buffer.data() + buffer.size(), value);
    out.append(buffer.data(), result.ptr);
}

void
    WriteFloat(std::string& out, double value) noexcept {
    if (!std::isfinite(value)) {
        // NaN and +-Infinity have no JSON representation. Writing `null`
        // keeps the output parseable instead of emitting invalid JSON.
        out.append("null");
        return;
    }

    // The shortest round-trip representation of a double never exceeds
    // this comfortably, even for values like -2.2250738585072014e-308.
    std::array<char, 32> buffer{};
    const auto result = std::to_chars(buffer.data(), buffer.data() + buffer.size(), value);
    const auto written = static_cast<size_t>(result.ptr - buffer.data());

    const std::string_view rendered{buffer.data(), written};
    const bool looks_like_float = rendered.find_first_of(".eE") != std::string_view::npos;

    out.append(rendered);
    if (!looks_like_float) {
        // to_chars can produce a bare integer string (e.g. "3" for 3.0).
        // Force a decimal point so this value re-parses as JsonType::Float
        // rather than JsonType::Integer.
        out.append(".0");
    }
}

} // namespace zuu::json::serializer