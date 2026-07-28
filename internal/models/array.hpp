/**
 * @file array.hpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Declares Array, the DOM node type for a JSON array value.
 * @version 0.4.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#pragma once

#include <cstddef>
#include <expected>
#include <span>
#include <type_traits>
#include <vector>

#include "models/value.hpp"
#include "zd_json/error.hpp"

namespace zuu::models {

class Arena;

/**
 * @brief An immutable, arena-owned sequence of JSON values.
 *
 * Array itself never allocates: instances are produced by Array::Builder
 * and always live inside a Document's Arena, so they're released in bulk
 * along with the rest of the tree rather than individually. Element
 * count isn't known upfront while parsing, so Builder stages elements in
 * an ordinary std::vector and only copies the final, fixed-size result
 * into the arena once Build() is called; that staging vector's heap
 * memory is freed as soon as Build() returns.
 */
class Array {
  public:
    /**
     * @brief Accumulates elements before committing them to an Arena.
     */
    class Builder {
      public:
        Builder() noexcept = default;

        void
            Push(Value value) noexcept;

        [[nodiscard]] size_t
            size() const noexcept {
            return staging_.size();
        }

        /**
         * @brief Copies the staged elements into @p arena and returns the
         *        resulting Array, also allocated in @p arena.
         *
         * @return Pointer to the finished Array, or nullptr if @p arena
         *         fails to allocate.
         */
        [[nodiscard]] Array*
            Build(Arena& arena) noexcept;

      private:
        std::vector<Value> staging_;
    };

    [[nodiscard]] std::span<const Value>
        values() const noexcept {
        return values_;
    }

    [[nodiscard]] size_t
        size() const noexcept {
        return values_.size();
    }

    [[nodiscard]] bool
        empty() const noexcept {
        return values_.empty();
    }

    /**
     * @return A pointer to the element at @p index, or JsonErrc::OutOfBound
     *         if @p index is out of range.
     */
    [[nodiscard]] std::expected<const Value*, JsonErrc>
        at(size_t index) const noexcept;

    [[nodiscard]] std::span<const Value>::iterator
        begin() const noexcept {
        return values_.begin();
    }

    [[nodiscard]] std::span<const Value>::iterator
        end() const noexcept {
        return values_.end();
    }

  private:
    friend class Builder;
    friend class Arena;

    constexpr explicit Array(std::span<const Value> values) noexcept
        : values_(values) {}

    std::span<const Value> values_;
};

static_assert(std::is_trivially_copyable_v<Array>, "Array must be trivially copyable.");

} // namespace zuu::models
