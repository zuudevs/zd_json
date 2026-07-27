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
};

} // namespace zuu::lexer