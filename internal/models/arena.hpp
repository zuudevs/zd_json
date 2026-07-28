/**
 * @file arena.hpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Declares Arena, the bump allocator backing the JSON DOM.
 * @version 0.4.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#pragma once

#include <cstddef>
#include <cstring>
#include <new>
#include <span>
#include <string_view>
#include <type_traits>
#include <utility>

namespace zuu::json::models {

/**
 * @brief A growable bump (arena) allocator that owns every node of a
 *        Document tree.
 *
 * Allocation is a pointer bump: each call carves the next `size` bytes off
 * the current block and advances an offset, with no per-object bookkeeping
 * and no way to free a single node. When a block runs out of room, a new
 * one is appended to a singly linked list; existing allocations are never
 * moved, so pointers and spans handed out by this arena stay valid for the
 * lifetime of the Arena. There is no per-object destructor tracking either
 * -- Arena is intended for the trivially destructible node types in
 * zuu::json::models (Value, Array, Object), so releasing memory in bulk on
 * Reset() or destruction never leaks a resource. Not thread-safe: an
 * Arena (and the Document that owns it) is meant to be built and read by
 * a single thread at a time.
 */
class Arena {
  public:
    /**
     * @brief Size, in bytes, of the first block allocated on demand when
     *        no explicit capacity is given.
     */
    static constexpr size_t kDefaultBlockSize = 4096;

    constexpr Arena() noexcept = default;

    /**
     * @brief Constructs an Arena whose blocks are at least
     *        @p initial_block_size bytes.
     *
     * @param initial_block_size Minimum size of each allocated block. A
     *        value of 0 falls back to kDefaultBlockSize.
     */
    explicit Arena(size_t initial_block_size) noexcept;

    ~Arena() noexcept;

    Arena(const Arena&) = delete;
    Arena&
        operator=(const Arena&) = delete;

    Arena(Arena&& other) noexcept;
    Arena&
        operator=(Arena&& other) noexcept;

    /**
     * @brief Bump-allocates a raw, uninitialized block of memory.
     *
     * @param size Number of bytes to reserve.
     * @param alignment Required alignment; must be a power of two.
     * @return Pointer to the reserved memory, or nullptr if @p size is 0
     *         or the underlying block allocation fails.
     */
    [[nodiscard]] void*
        Allocate(size_t size, size_t alignment) noexcept;

    /**
     * @brief Constructs a T in the arena and returns a pointer to it.
     *
     * T's constructor should be non-throwing: Create() is itself noexcept,
     * so a throwing constructor would terminate the program rather than
     * propagate. All node types in zuu::json::models (Value, Array, Object) use
     * only noexcept constructors for this reason.
     *
     * @param args Arguments forwarded to T's constructor.
     * @return Pointer to the constructed T, or nullptr on allocation
     *         failure.
     */
    template <typename T, typename... Args>
    [[nodiscard]] T*
        Create(Args&&... args) noexcept {
        void* memory = Allocate(sizeof(T), alignof(T));
        if (memory == nullptr) {
            return nullptr;
        }

        // Placement-new is used instead of std::construct_at because the
        // latter constructs the object from inside a library function, not
        // from inside Arena's own code -- so a `friend class Arena;`
        // declaration (used by Array/Object to keep their constructors
        // private) wouldn't grant it access.
        return ::new (memory) T(std::forward<Args>(args)...);
    }

    /**
     * @brief Copies @p source into arena-owned storage and returns a span
     *        over the copy.
     *
     * Used to finalize the staging buffers built by Array::Builder and
     * Object::Builder into their permanent, arena-owned form.
     *
     * @param source Elements to copy. T must be trivially copyable so the
     *        copy can be a single memcpy.
     * @return A span over the arena-owned copy, or an empty span if
     *         @p source is empty or allocation fails.
     */
    template <typename T>
    [[nodiscard]] std::span<T>
        CreateArray(std::span<const T> source) noexcept {
        static_assert(std::is_trivially_copyable_v<T>,
                      "Arena::CreateArray requires a trivially copyable element type.");

        if (source.empty()) {
            return {};
        }

        void* memory = Allocate(sizeof(T) * source.size(), alignof(T));
        if (memory == nullptr) {
            return {};
        }

        auto* first = static_cast<T*>(memory);
        std::memcpy(first, source.data(), sizeof(T) * source.size());
        return std::span<T>{first, source.size()};
    }

    /**
     * @brief Copies @p source into arena-owned storage.
     *
     * Use this to give a DOM Value a string that is guaranteed to live as
     * long as the Document, instead of a view into a transient or
     * caller-owned buffer.
     *
     * @param source Bytes to copy.
     * @return A view over the arena-owned copy, or an empty view if
     *         @p source is empty or allocation fails.
     */
    [[nodiscard]] std::string_view
        CreateString(std::string_view source) noexcept;

    /**
     * @brief Releases every block and resets the arena to empty.
     *
     * Invalidates all pointers and spans previously handed out by this
     * arena.
     */
    void
        Reset() noexcept;

    /**
     * @brief Total number of bytes handed out via Allocate so far.
     */
    [[nodiscard]] size_t
        bytes_allocated() const noexcept {
        return bytes_allocated_;
    }

    /**
     * @brief Number of blocks currently backing this arena.
     */
    [[nodiscard]] size_t
        block_count() const noexcept;

  private:
    struct Block {
        std::byte* memory;
        size_t capacity;
        size_t used;
        Block* next;
    };

    [[nodiscard]] Block*
        AllocateBlock(size_t min_capacity) noexcept;
    void
        ReleaseBlocks(Block* block) noexcept;

    Block* head_ = nullptr;
    size_t default_block_size_ = kDefaultBlockSize;
    size_t bytes_allocated_ = 0;
};

} // namespace zuu::json::models
