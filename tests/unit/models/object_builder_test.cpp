/**
 * @file object_builder_test.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Unit tests for Object::Builder and Object operations.
 * @version 0.7.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include "json_models_test.hpp"

TEST(JsonModelsTest, ObjectBuilder) {
    Arena arena;

    Object::Builder builder;
    builder.Insert("name", Value{std::string_view{"zuu"}});
    builder.Insert("age", Value{static_cast<int64_t>(20)});
    builder.Insert("active", Value{true});
    EXPECT_EQ(builder.size(), 3u);

    Object* object = builder.Build(arena);
    ASSERT_NE(object, nullptr);
    EXPECT_EQ(object->size(), 3u);

    EXPECT_TRUE(object->Contains("name"));
    EXPECT_EQ(object->Find("name").value()->as_string().value(), "zuu");
    EXPECT_EQ(object->Find("age").value()->as_integer().value(), 20);
    EXPECT_EQ(object->Find("active").value()->as_bool().value(), true);

    EXPECT_FALSE(object->Contains("missing"));
    EXPECT_FALSE(object->Find("missing").has_value());
    EXPECT_EQ(object->Find("missing").error(), JsonErrc::OutOfBound);
}
