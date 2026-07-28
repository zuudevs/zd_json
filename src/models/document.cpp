/**
 * @file document.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Implements Document.
 * @version 0.4.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include "models/document.hpp"

namespace zuu::json::models {

Document::Document(size_t initial_block_size) noexcept
    : arena_(initial_block_size) {}

void
    Document::Reset() noexcept {
    arena_.Reset();
    root_ = Value{};
}

} // namespace zuu::json::models
