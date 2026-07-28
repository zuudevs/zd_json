/**
 * @file scan_result.hpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Defines the ScanResult structure.
 * @version 0.3.0
 * @date 2026-07-27
 *
 * @copyright Copyright (c) 2026
 */

#pragma once

#include <cstddef>

#include "enums/json_type.hpp"
#include "zd_json/error.hpp"

namespace zuu::lexer {

/**
 * @brief Result of a single forward scan over a JSON value.
 *
 * Stores the detected JSON value type and the exclusive end index
 * of the scanned value within the input buffer.
 */
struct ScanResult {
    /**
     * @brief The detected JSON value type.
     */
    enums::JsonType type;

    /**
     * @brief Exclusive end index of the scanned value.
     *
     * This is the index immediately following the last character
     * that belongs to the scanned JSON value.
     */
    size_t value_end;

    /**
     * @brief First validation error found while scanning, if any.
     *
     * ScanString sets this to a non-None value when it encounters an
     * unescaped control character or a malformed UTF-8 byte sequence.
     * Scanning still continues to the true end of the value so that
     * @c value_end remains correct; only the first error is kept.
     */
    JsonErrc error = JsonErrc::None;
};

} // namespace zuu::lexer