/**
 * @file parse_test.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Unit tests for zuu::json::jsonParse.
 * @version 0.2.0
 * @date 2026-07-29
 *
 * @copyright Copyright (c) 2026
 */

#include "json_api_test.hpp"

TEST(JsonApiTest, ParseObject) {
    auto document = json::Parse(R"({"name": "zuu", "count": 3, "ok": true, "extra": null})");
    ASSERT_TRUE(document.has_value());

    const json::Value& root = document->root();
    ASSERT_TRUE(root.is_object());

    const json::Object* object = root.as_object().value();
    EXPECT_EQ(object->Find("name").value()->as_string().value(), "zuu");
    EXPECT_EQ(object->Find("count").value()->as_integer().value(), 3);
    EXPECT_EQ(object->Find("ok").value()->as_bool().value(), true);
    EXPECT_TRUE(object->Find("extra").value()->is_null());
}

TEST(JsonApiTest, ParseArray) {
    auto document = json::Parse("[1, 2, 3, 4]");
    ASSERT_TRUE(document.has_value());

    const json::Array* array = document->root().as_array().value();
    ASSERT_EQ(array->size(), 4u);
    EXPECT_EQ(array->at(0).value()->as_integer().value(), 1);
    EXPECT_EQ(array->at(3).value()->as_integer().value(), 4);
}

TEST(JsonApiTest, ParseScalar) {
    auto document = json::Parse("  42  ");
    ASSERT_TRUE(document.has_value());
    EXPECT_EQ(document->root().as_integer().value(), 42);
}

TEST(JsonApiTest, ParseSurroundingWhitespaceIsIgnored) {
    auto document = json::Parse("\n\t  true  \n");
    ASSERT_TRUE(document.has_value());
    EXPECT_EQ(document->root().as_bool().value(), true);
}

TEST(JsonApiTest, ParseEmptyInputFails) {
    auto document = json::Parse("");
    ASSERT_FALSE(document.has_value());
    EXPECT_EQ(document.error(), json::JsonErrc::EmptyValue);
}

TEST(JsonApiTest, ParseWhitespaceOnlyInputFails) {
    auto document = json::Parse("   \n  ");
    ASSERT_FALSE(document.has_value());
    EXPECT_EQ(document.error(), json::JsonErrc::EmptyValue);
}

TEST(JsonApiTest, ParseTrailingGarbageFails) {
    auto document = json::Parse("123 abc");
    ASSERT_FALSE(document.has_value());
    EXPECT_EQ(document.error(), json::JsonErrc::InvalidFormat);
}

TEST(JsonApiTest, ParseTrailingGarbageAfterContainerFails) {
    auto document = json::Parse("[1, 2] [3]");
    ASSERT_FALSE(document.has_value());
    EXPECT_EQ(document.error(), json::JsonErrc::InvalidFormat);
}

TEST(JsonApiTest, ParseMalformedInputPropagatesUnderlyingError) {
    auto document = json::Parse(R"({"key": })");
    ASSERT_FALSE(document.has_value());
    EXPECT_NE(document.error(), json::JsonErrc::None);
}

TEST(JsonApiTest, ParseRespectsInitialBlockSizeOption) {
    json::ParseOptions options;
    options.initial_block_size = 64;

    auto document = Parse(R"({"a": 1})", options);
    ASSERT_TRUE(document.has_value());
    EXPECT_EQ(document->root().as_object().value()->Find("a").value()->as_integer().value(), 1);
}