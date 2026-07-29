/**
 * @file arena_reset_test.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Unit tests for Arena Reset functionality.
 * @version 0.7.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include "json_models_test.hpp"

TEST(JsonModelsTest, ArenaReset) {
    Arena arena;
    (void)arena.Create<int>(1);
    (void)arena.Create<int>(2);

    EXPECT_GT(arena.bytes_allocated(), 0u);

    arena.Reset();

    EXPECT_EQ(arena.bytes_allocated(), 0u);
    EXPECT_EQ(arena.block_count(), 0u);

    // The arena must still be usable after Reset().
    int* value = arena.Create<int>(7);
    ASSERT_NE(value, nullptr);
    EXPECT_EQ(*value, 7);
}
