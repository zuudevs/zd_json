/**
 * @file serialize_compact_containers_test.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Unit tests for SerializeCompact on containers.
 * @version 0.7.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include "json_serializer_test.hpp"

TEST(JsonSerializerTest, SerializeCompactContainers) {
    Document document;

    // Empty containers.
    {
        Array::Builder builder;
        Array* array = builder.Build(document.arena());
        EXPECT_EQ(SerializeCompact(Value{array}), "[]");
    }
    {
        Object::Builder builder;
        Object* object = builder.Build(document.arena());
        EXPECT_EQ(SerializeCompact(Value{object}), "{}");
    }

    // Flat array of mixed scalar types.
    {
        Array::Builder builder;
        builder.Push(Value{static_cast<int64_t>(1)});
        builder.Push(Value{2.5});
        builder.Push(Value{document.MakeString("three")});
        builder.Push(Value{true});
        builder.Push(Value{false});
        builder.Push(Value{nullptr});
        Array* array = builder.Build(document.arena());
        EXPECT_EQ(SerializeCompact(Value{array}), R"([1,2.5,"three",true,false,null])");
    }

    // Flat object; members must render in insertion order.
    {
        Object::Builder builder;
        builder.Insert("name", Value{document.MakeString("zuu")});
        builder.Insert("age", Value{static_cast<int64_t>(20)});
        builder.Insert("active", Value{true});
        Object* object = builder.Build(document.arena());
        EXPECT_EQ(SerializeCompact(Value{object}), R"({"name":"zuu","age":20,"active":true})");
    }

    // Nested arrays and objects together.
    {
        constexpr auto jsonPayload =
            R"({"values":[1,2,3],"nested":{"ok":true,"inner":{"deep":1}}})";
        Arena arena;
        size_t readPos = 0;
        const auto parseResult =
            ParseValue(jsonPayload, readPos, std::strlen(jsonPayload), arena, 0);
        ASSERT_TRUE(parseResult.has_value());
        EXPECT_EQ(SerializeCompact(*parseResult), jsonPayload);
    }

    // Object keys containing characters that need escaping.
    {
        Object::Builder builder;
        builder.Insert(document.MakeString("line\nbreak"), Value{static_cast<int64_t>(1)});
        Object* object = builder.Build(document.arena());
        EXPECT_EQ(SerializeCompact(Value{object}), R"({"line\nbreak":1})");
    }

    // Duplicate keys are preserved.
    {
        Object::Builder builder;
        builder.Insert("a", Value{static_cast<int64_t>(1)});
        builder.Insert("a", Value{static_cast<int64_t>(2)});
        Object* object = builder.Build(document.arena());
        EXPECT_EQ(SerializeCompact(Value{object}), R"({"a":1,"a":2})");
    }
}
