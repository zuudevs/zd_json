/**
 * @file serialize_pretty_containers_test.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Unit tests for SerializePretty on containers.
 * @version 0.7.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include "json_serializer_test.hpp"

TEST(JsonSerializerTest, SerializePrettyContainers) {
    // Empty containers stay on one line even in pretty mode.
    {
        Document document;
        Array::Builder builder;
        Array* array = builder.Build(document.arena());
        EXPECT_EQ(SerializePretty(Value{array}), "[]");
    }
    {
        Document document;
        Object::Builder builder;
        Object* object = builder.Build(document.arena());
        EXPECT_EQ(SerializePretty(Value{object}), "{}");
    }

    // Flat array, default 2-space indent.
    {
        Document document;
        Array::Builder builder;
        builder.Push(Value{static_cast<int64_t>(1)});
        builder.Push(Value{static_cast<int64_t>(2)});
        builder.Push(Value{static_cast<int64_t>(3)});
        Array* array = builder.Build(document.arena());
        const std::string expectedStr = "[\n  1,\n  2,\n  3\n]";
        EXPECT_EQ(SerializePretty(Value{array}), expectedStr);
    }

    // Flat object, default 2-space indent, space after ':'.
    {
        Document document;
        Object::Builder builder;
        builder.Insert("a", Value{static_cast<int64_t>(1)});
        builder.Insert("b", Value{true});
        Object* object = builder.Build(document.arena());
        const std::string expectedStr = "{\n  \"a\": 1,\n  \"b\": true\n}";
        EXPECT_EQ(SerializePretty(Value{object}), expectedStr);
    }

    // Nested structure indents one extra level per nesting depth.
    {
        constexpr auto jsonPayload = R"({"a":[1,2],"b":{"c":3}})";
        Arena arena;
        size_t readPos = 0;
        const auto parseResult =
            ParseValue(jsonPayload, readPos, std::strlen(jsonPayload), arena, 0);
        ASSERT_TRUE(parseResult.has_value());
        const std::string expectedStr = "{\n"
                                        "  \"a\": [\n"
                                        "    1,\n"
                                        "    2\n"
                                        "  ],\n"
                                        "  \"b\": {\n"
                                        "    \"c\": 3\n"
                                        "  }\n"
                                        "}";
        EXPECT_EQ(SerializePretty(*parseResult), expectedStr);
    }

    // Empty container nested inside a non-empty one.
    {
        constexpr auto jsonPayload = R"({"a":[]})";
        Arena arena;
        size_t readPos = 0;
        const auto parseResult =
            ParseValue(jsonPayload, readPos, std::strlen(jsonPayload), arena, 0);
        ASSERT_TRUE(parseResult.has_value());
        EXPECT_EQ(SerializePretty(*parseResult), "{\n  \"a\": []\n}");
    }

    // Custom indent size.
    {
        Document document;
        Object::Builder builder;
        builder.Insert("a", Value{static_cast<int64_t>(1)});
        Object* object = builder.Build(document.arena());
        EXPECT_EQ(SerializePretty(Value{object}, 4), "{\n    \"a\": 1\n}");
    }

    // indent_size of 0.
    {
        Document document;
        Object::Builder builder;
        builder.Insert("a", Value{static_cast<int64_t>(1)});
        Object* object = builder.Build(document.arena());
        EXPECT_EQ(SerializePretty(Value{object}, 0), "{\n\"a\": 1\n}");
    }
}
