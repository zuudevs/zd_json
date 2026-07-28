/**
 * @file serialize_number.hpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Declares WriteInteger and WriteFloat for formatting JSON numbers.
 * @version 0.6.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#pragma once

#include <cstdint>
#include <string>

namespace zuu::serializer {

/**
 * @brief Appends the base-10 text form of @p value to @p out.
 *
 * @param out Buffer to append to.
 * @param value Integer to format.
 */
void
    WriteInteger(std::string& out, int64_t value) noexcept;

/**
 * @brief Appends the shortest round-trippable text form of @p value to
 *        @p out.
 *
 * Uses std::to_chars to produce the shortest decimal string that reads
 * back to the exact same double, then guarantees the result still looks
 * like a JSON *float* rather than an integer by appending `.0` when
 * to_chars produced neither a decimal point nor an exponent -- otherwise
 * a value like 3.0 would round-trip through this library's parser as
 * JsonType::Integer instead of JsonType::Float. Non-finite values (NaN,
 * +-Infinity) have no JSON representation; they are written as `null`
 * rather than producing text that wouldn't parse back as JSON.
 *
 * @param out Buffer to append to.
 * @param value Floating-point value to format.
 */
void
    WriteFloat(std::string& out, double value) noexcept;

} // namespace zuu::serializer