/**
 * @file arena.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Implements Arena.
 * @version 0.4.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include "models/arena.hpp"

#include <algorithm>
#include <new>

namespace zuu::json::models {

namespace {

[[nodiscard]] constexpr size_t
    AlignUp(size_t value, size_t alignment) noexcept {
    return (value + (alignment - 1)) & ~(alignment - 1);
}

} // namespace

Arena::Arena(size_t initial_block_size) noexcept
    : default_block_size_(initial_block_size == 0 ? kDefaultBlockSize : initial_block_size) {}

Arena::~Arena() noexcept {
    ReleaseBlocks(head_);
}

Arena::Arena(Arena&& other) noexcept
    : head_(other.head_)
    , default_block_size_(other.default_block_size_)
    , bytes_allocated_(other.bytes_allocated_) {
    other.head_ = nullptr;
    other.bytes_allocated_ = 0;
}

Arena&
    Arena::operator=(Arena&& other) noexcept {
    if (this != &other) {
        ReleaseBlocks(head_);

        head_ = other.head_;
        default_block_size_ = other.default_block_size_;
        bytes_allocated_ = other.bytes_allocated_;

        other.head_ = nullptr;
        other.bytes_allocated_ = 0;
    }
    return *this;
}

void*
    Arena::Allocate(size_t size, size_t alignment) noexcept {
    if (size == 0) {
        return nullptr;
    }

    if (head_ != nullptr) {
        const size_t aligned_used = AlignUp(head_->used, alignment);
        if (aligned_used + size <= head_->capacity) {
            std::byte* ptr = head_->memory + aligned_used;
            head_->used = aligned_used + size;
            bytes_allocated_ += size;
            return ptr;
        }
    }

    // The current block (if any) doesn't have enough room; append a new
    // one sized to fit at least this allocation. A block always starts
    // fully unused, and operator new already aligns to at least
    // alignof(std::max_align_t), so the first allocation out of a fresh
    // block needs no extra alignment padding.
    Block* block = AllocateBlock(std::max(size, default_block_size_));
    if (block == nullptr) {
        return nullptr;
    }

    std::byte* ptr = block->memory;
    block->used = size;
    bytes_allocated_ += size;
    return ptr;
}

std::string_view
    Arena::CreateString(std::string_view source) noexcept {
    if (source.empty()) {
        return {};
    }

    void* memory = Allocate(source.size(), alignof(char));
    if (memory == nullptr) {
        return {};
    }

    std::memcpy(memory, source.data(), source.size());
    return std::string_view{static_cast<const char*>(memory), source.size()};
}

void
    Arena::Reset() noexcept {
    ReleaseBlocks(head_);
    head_ = nullptr;
    bytes_allocated_ = 0;
}

size_t
    Arena::block_count() const noexcept {
    size_t count = 0;
    for (const Block* block = head_; block != nullptr; block = block->next) {
        ++count;
    }
    return count;
}

Arena::Block*
    Arena::AllocateBlock(size_t min_capacity) noexcept {
    auto* memory = static_cast<std::byte*>(::operator new(min_capacity, std::nothrow));
    if (memory == nullptr) {
        return nullptr;
    }

    auto* block = static_cast<Block*>(::operator new(sizeof(Block), std::nothrow));
    if (block == nullptr) {
        ::operator delete(memory);
        return nullptr;
    }

    block->memory = memory;
    block->capacity = min_capacity;
    block->used = 0;
    block->next = head_;
    head_ = block;
    return block;
}

void
    Arena::ReleaseBlocks(Block* block) noexcept {
    while (block != nullptr) {
        Block* next = block->next;
        ::operator delete(block->memory);
        ::operator delete(block);
        block = next;
    }
}

} // namespace zuu::json::models
