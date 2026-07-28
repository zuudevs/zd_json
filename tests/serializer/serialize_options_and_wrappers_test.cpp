/**
 * @file serialize_options_and_wrappers_test.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Unit tests for Serialize options and helper functions.
 * @version 0.7.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include "json_serializer_test.hpp"

TEST(JsonSerializerTest, SerializeOptionsAndWrappers) {
    Document document;
    Object::Builder builder;
    builder.Insert("a", Value{static_cast<int64_t>(1)});
    Object* object = builder.Build(document.arena());
    const Value rootValue{object};

    // Default-constructed options are compact.
    EXPECT_EQ(Serialize(rootValue), R"({"a":1})");
    EXPECT_EQ(Serialize(rootValue, SerializeOptions{}), R"({"a":1})");

    // SerializeCompact matches explicit compact options.
    EXPECT_EQ(SerializeCompact(rootValue), Serialize(rootValue, SerializeOptions{.pretty = false}));

    // SerializePretty matches explicit pretty options.
    EXPECT_EQ(SerializePretty(rootValue),
              Serialize(rootValue, SerializeOptions{.pretty = true, .indent_size = 2}));
    EXPECT_EQ(SerializePretty(rootValue, 4),
              Serialize(rootValue, SerializeOptions{.pretty = true, .indent_size = 4}));
}
