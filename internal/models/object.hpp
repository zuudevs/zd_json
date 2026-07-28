/**
 * @file object.hpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Declares Object, the DOM node type for a JSON object value.
 * @version 0.4.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#pragma once

#include <cstddef>
#include <expected>
#include <span>
#include <string_view>
#include <type_traits>
#include <vector>

#include "models/value.hpp"
#include "zd_json/error.hpp"

namespace zuu::models {

class Arena;

/**
 * @brief An immutable, arena-owned collection of JSON object members.
 *
 * Like Array, Object never allocates itself: instances are produced by
 * Object::Builder and always live inside a Document's Arena. Members
 * keep their insertion order and duplicate keys are permitted (as JSON's
 * grammar allows); Find() returns the first match, mirroring how most
 * JSON implementations resolve duplicates. Lookup is a linear scan --
 * appropriate for the small member counts typical of JSON objects, and
 * consistent with this library favoring simple, cache-friendly layouts
 * over auxiliary index structures.
 */
class Object {
  public:
    /**
     * @brief A single `"key": value` pair.
     */
    struct Member {
        std::string_view key;
        Value value;
    };

    /**
     * @brief Accumulates members before committing them to an Arena.
     */
    class Builder {
      public:
        Builder() noexcept = default;

        void
            Insert(std::string_view key, Value value) noexcept;

        [[nodiscard]] size_t
            size() const noexcept {
            return staging_.size();
        }

        /**
         * @brief Copies the staged members into @p arena and returns the
         *        resulting Object, also allocated in @p arena.
         *
         * @return Pointer to the finished Object, or nullptr if @p arena
         *         fails to allocate.
         */
        [[nodiscard]] Object*
            Build(Arena& arena) noexcept;

      private:
        std::vector<Member> staging_;
    };

    [[nodiscard]] std::span<const Member>
        members() const noexcept {
        return members_;
    }

    [[nodiscard]] size_t
        size() const noexcept {
        return members_.size();
    }

    [[nodiscard]] bool
        empty() const noexcept {
        return members_.empty();
    }

    /**
     * @return A pointer to the value for @p key, or JsonErrc::OutOfBound
     *         if no member with that key exists.
     */
    [[nodiscard]] std::expected<const Value*, JsonErrc>
        Find(std::string_view key) const noexcept;

    [[nodiscard]] bool
        Contains(std::string_view key) const noexcept;

    [[nodiscard]] std::span<const Member>::iterator
        begin() const noexcept {
        return members_.begin();
    }

    [[nodiscard]] std::span<const Member>::iterator
        end() const noexcept {
        return members_.end();
    }

  private:
    friend class Builder;
    friend class Arena;

    constexpr explicit Object(std::span<const Member> members) noexcept
        : members_(members) {}

    std::span<const Member> members_;
};

static_assert(std::is_trivially_copyable_v<Object::Member>,
              "Object::Member must be trivially copyable so Arena::CreateArray can memcpy it.");

} // namespace zuu::models
