/**
 * @file array.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Implements Array and Array::Builder.
 * @version 0.4.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include "models/array.hpp"

#include "models/arena.hpp"

namespace zuu::models {

void
    Array::Builder::Push(Value value) noexcept {
    staging_.push_back(value);
}

Array*
    Array::Builder::Build(Arena& arena) noexcept {
    const std::span<const Value> values = arena.CreateArray<Value>(staging_);
    return arena.Create<Array>(values);
}

std::expected<const Value*, JsonErrc>
    Array::at(size_t index) const noexcept {
    if (index >= values_.size()) {
        return std::unexpected{JsonErrc::OutOfBound};
    }
    return &values_[index];
}

} // namespace zuu::models
