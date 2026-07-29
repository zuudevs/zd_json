/**
 * @file arena_grows_test.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Unit tests for Arena growing across multiple blocks.
 * @version 0.7.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include "json_models_test.hpp"

TEST(JsonModelsTest, ArenaGrowsAcrossBlocks) {
    // Force at least one extra block by requesting more than a single
    // default-sized block can hold.
    Arena arena(64);

    for (int i = 0; i < 1000; ++i) {
        int* value = arena.Create<int>(i);
        ASSERT_NE(value, nullptr);
        EXPECT_EQ(*value, i);
    }

    EXPECT_GT(arena.block_count(), 1u);
}
