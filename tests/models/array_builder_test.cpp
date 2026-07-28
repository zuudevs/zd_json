/**
 * @file array_builder_test.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Unit tests for Array::Builder and Array operations.
 * @version 0.7.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include "json_models_test.hpp"

TEST(JsonModelsTest, ArrayBuilder) {
    Arena arena;

    Array::Builder builder;
    builder.Push(Value{static_cast<int64_t>(1)});
    builder.Push(Value{static_cast<int64_t>(2)});
    builder.Push(Value{static_cast<int64_t>(3)});
    EXPECT_EQ(builder.size(), 3u);

    Array* array = builder.Build(arena);
    ASSERT_NE(array, nullptr);
    EXPECT_EQ(array->size(), 3u);
    EXPECT_FALSE(array->empty());

    EXPECT_EQ(array->at(0).value()->as_integer().value(), 1);
    EXPECT_EQ(array->at(1).value()->as_integer().value(), 2);
    EXPECT_EQ(array->at(2).value()->as_integer().value(), 3);

    EXPECT_FALSE(array->at(3).has_value());
    EXPECT_EQ(array->at(3).error(), JsonErrc::OutOfBound);

    int64_t sum = 0;
    for (const Value& element : *array) {
        sum += element.as_integer().value();
    }
    EXPECT_EQ(sum, 6);

    // An empty array is a valid, well-formed Array.
    Array::Builder empty_builder;
    Array* empty_array = empty_builder.Build(arena);
    ASSERT_NE(empty_array, nullptr);
    EXPECT_TRUE(empty_array->empty());
    EXPECT_EQ(empty_array->size(), 0u);
}
