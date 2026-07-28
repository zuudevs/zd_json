/**
 * @file document.hpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Declares Document, the owning root of a parsed JSON DOM tree.
 * @version 0.4.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#pragma once

#include <cstddef>
#include <string_view>

#include "models/arena.hpp"
#include "models/value.hpp"

namespace zuu::models {

/**
 * @brief Owns the memory for a parsed JSON document tree.
 *
 * A Document bundles an Arena with a root Value. Every Array and Object
 * reachable from the root is allocated out of that same Arena, so the
 * whole tree is released in one shot -- without walking it -- when the
 * Document is destroyed or Reset() is called. Every Value, Array, and
 * Object produced through a Document is only valid for as long as that
 * Document stays alive.
 */
class Document {
  public:
    constexpr Document() noexcept = default;

    /**
     * @brief Constructs a Document whose Arena blocks are at least
     *        @p initial_block_size bytes.
     */
    explicit Document(size_t initial_block_size) noexcept;

    Document(const Document&) = delete;
    Document&
        operator=(const Document&) = delete;

    Document(Document&&) noexcept = default;
    Document&
        operator=(Document&&) noexcept = default;

    [[nodiscard]] Arena&
        arena() noexcept {
        return arena_;
    }

    [[nodiscard]] const Arena&
        arena() const noexcept {
        return arena_;
    }

    [[nodiscard]] const Value&
        root() const noexcept {
        return root_;
    }

    /**
     * @brief Sets the document's root value.
     *
     * If @p value wraps an Array pointer, Object pointer, or string view,
     * the caller is responsible for ensuring that memory came from (or
     * outlives) this Document's Arena -- see arena(), Array::Builder,
     * Object::Builder, and MakeString().
     */
    void
        set_root(Value value) noexcept {
        root_ = value;
    }

    /**
     * @brief Copies @p value into this document's arena and returns a
     *        view suitable for building an owned string Value.
     */
    [[nodiscard]] std::string_view
        MakeString(std::string_view value) noexcept {
        return arena_.CreateString(value);
    }

    /**
     * @brief Releases the entire tree and resets the document to an
     *        empty (null root) state.
     */
    void
        Reset() noexcept;

  private:
    Arena arena_;
    Value root_{};
};

} // namespace zuu::models
