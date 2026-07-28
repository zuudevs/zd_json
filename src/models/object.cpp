/**
 * @file object.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Implements Object and Object::Builder.
 * @version 0.4.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include "models/object.hpp"

#include "models/arena.hpp"

namespace zuu::models {

void
    Object::Builder::Insert(std::string_view key, Value value) noexcept {
    staging_.push_back(Member{key, value});
}

Object*
    Object::Builder::Build(Arena& arena) noexcept {
    const std::span<const Member> members = arena.CreateArray<Member>(staging_);
    return arena.Create<Object>(members);
}

std::expected<const Value*, JsonErrc>
    Object::Find(std::string_view key) const noexcept {
    for (const Member& member : members_) {
        if (member.key == key) {
            return &member.value;
        }
    }
    return std::unexpected{JsonErrc::OutOfBound};
}

bool
    Object::Contains(std::string_view key) const noexcept {
    return Find(key).has_value();
}

} // namespace zuu::models
