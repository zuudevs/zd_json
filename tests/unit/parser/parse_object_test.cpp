/**
 * @file parse_object_test.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Unit tests for ParseObject.
 * @version 0.7.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include "json_parser_test.hpp"

TEST(JsonParserTest, ParseObjectEmpty) {
    Arena arena;
    constexpr auto jsonPayload = "{}";
    size_t pos = 0;
    const auto parseResult = ParseObject(jsonPayload, pos, std::strlen(jsonPayload), arena, 0);
    ASSERT_TRUE(parseResult.has_value());
    EXPECT_TRUE((*parseResult)->empty());
    EXPECT_EQ(pos, std::strlen(jsonPayload));
}

TEST(JsonParserTest, ParseObjectFlatMixed) {
    Arena arena;
    constexpr auto jsonPayload =
        R"({"name": "zuu", "age": 20, "score": 98.5, "active": true, "extra": null})";
    size_t pos = 0;
    const auto parseResult = ParseObject(jsonPayload, pos, std::strlen(jsonPayload), arena, 0);
    ASSERT_TRUE(parseResult.has_value());
    const Object* object = *parseResult;
    EXPECT_EQ(object->size(), 5u);
    EXPECT_EQ(object->Find("name").value()->as_string().value(), "zuu");
    EXPECT_EQ(object->Find("age").value()->as_integer().value(), 20);
    EXPECT_TRUE(approx_equal(object->Find("score").value()->as_double().value(), 98.5));
    EXPECT_EQ(object->Find("active").value()->as_bool().value(), true);
    EXPECT_TRUE(object->Find("extra").value()->is_null());
    EXPECT_EQ(pos, std::strlen(jsonPayload));
}

TEST(JsonParserTest, ParseObjectNested) {
    Arena arena;
    constexpr auto jsonPayload =
        R"({"values": [1, 2, 3], "nested": {"ok": true, "inner": {"deep": 1}}})";
    size_t pos = 0;
    const auto parseResult = ParseObject(jsonPayload, pos, std::strlen(jsonPayload), arena, 0);
    ASSERT_TRUE(parseResult.has_value());
    const Object* object = *parseResult;

    const Array* values = object->Find("values").value()->as_array().value();
    EXPECT_EQ(values->size(), 3u);
    EXPECT_EQ(values->at(2).value()->as_integer().value(), 3);

    const Object* nested = object->Find("nested").value()->as_object().value();
    EXPECT_EQ(nested->Find("ok").value()->as_bool().value(), true);

    const Object* inner = nested->Find("inner").value()->as_object().value();
    EXPECT_EQ(inner->Find("deep").value()->as_integer().value(), 1);
}

TEST(JsonParserTest, ParseObjectEscapedKey) {
    Arena arena;
    constexpr auto jsonPayload = R"({"line\nbreak": 1})";
    size_t pos = 0;
    const auto parseResult = ParseObject(jsonPayload, pos, std::strlen(jsonPayload), arena, 0);
    ASSERT_TRUE(parseResult.has_value());
    EXPECT_EQ((*parseResult)->Find("line\nbreak").value()->as_integer().value(), 1);
}

TEST(JsonParserTest, ParseObjectDuplicateKeys) {
    Arena arena;
    constexpr auto jsonPayload = R"({"a": 1, "a": 2})";
    size_t pos = 0;
    const auto parseResult = ParseObject(jsonPayload, pos, std::strlen(jsonPayload), arena, 0);
    ASSERT_TRUE(parseResult.has_value());
    EXPECT_EQ((*parseResult)->size(), 2u);
    EXPECT_EQ((*parseResult)->Find("a").value()->as_integer().value(), 1);
}

TEST(JsonParserTest, ParseObjectMalformed) {
    Arena arena;

    {
        const char* strPayload = R"("a": 1})";
        size_t pos = 0;
        const auto parseResult = ParseObject(strPayload, pos, std::strlen(strPayload), arena, 0);
        EXPECT_FALSE(parseResult.has_value());
        EXPECT_EQ(parseResult.error(), JsonErrc::InvalidFormat);
    }
    {
        const char* strPayload = R"({"a": 1)";
        size_t pos = 0;
        const auto parseResult = ParseObject(strPayload, pos, std::strlen(strPayload), arena, 0);
        EXPECT_FALSE(parseResult.has_value());
        EXPECT_EQ(parseResult.error(), JsonErrc::InvalidFormat);
    }
    {
        const char* strPayload = R"({a: 1})";
        size_t pos = 0;
        const auto parseResult = ParseObject(strPayload, pos, std::strlen(strPayload), arena, 0);
        EXPECT_FALSE(parseResult.has_value());
        EXPECT_EQ(parseResult.error(), JsonErrc::UnquotedKey);
    }
    {
        const char* strPayload = "{'a': 1}";
        size_t pos = 0;
        const auto parseResult = ParseObject(strPayload, pos, std::strlen(strPayload), arena, 0);
        EXPECT_FALSE(parseResult.has_value());
        EXPECT_EQ(parseResult.error(), JsonErrc::SingleQuotedString);
    }
    {
        const char* strPayload = R"({"a" 1})";
        size_t pos = 0;
        const auto parseResult = ParseObject(strPayload, pos, std::strlen(strPayload), arena, 0);
        EXPECT_FALSE(parseResult.has_value());
        EXPECT_EQ(parseResult.error(), JsonErrc::InvalidFormat);
    }
    {
        const char* strPayload = R"({"a": 1,})";
        size_t pos = 0;
        const auto parseResult = ParseObject(strPayload, pos, std::strlen(strPayload), arena, 0);
        EXPECT_FALSE(parseResult.has_value());
        EXPECT_EQ(parseResult.error(), JsonErrc::TrailingComma);
    }
    {
        const char* strPayload = R"({"a": 1 "b": 2})";
        size_t pos = 0;
        const auto parseResult = ParseObject(strPayload, pos, std::strlen(strPayload), arena, 0);
        EXPECT_FALSE(parseResult.has_value());
        EXPECT_EQ(parseResult.error(), JsonErrc::MissingComma);
    }
}
