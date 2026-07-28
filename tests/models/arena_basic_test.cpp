/**
 * @file arena_basic_test.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Unit tests for basic Arena operations.
 * @version 0.7.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include "json_models_test.hpp"

TEST(JsonModelsTest, ArenaBasic) {
    Arena arena;

    int* intPtr = arena.Create<int>(42);
    ASSERT_NE(intPtr, nullptr);
    EXPECT_EQ(*intPtr, 42);

    double* doublePtr = arena.Create<double>(3.5);
    ASSERT_NE(doublePtr, nullptr);
    EXPECT_EQ(*doublePtr, 3.5);

    // Distinct allocations must never alias.
    EXPECT_NE(static_cast<void*>(intPtr), static_cast<void*>(doublePtr));

    const std::string_view copiedString = arena.CreateString("hello arena");
    EXPECT_EQ(copiedString, "hello arena");
    // The string must have been copied, not merely referenced.
    EXPECT_NE(copiedString.data(), std::string_view{"hello arena"}.data());

    EXPECT_GT(arena.bytes_allocated(), 0u);
    EXPECT_GE(arena.block_count(), 1u);
}
