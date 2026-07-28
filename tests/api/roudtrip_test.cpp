/**
 * @file roundtrip_test.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Round-trip and serializer-wrapper tests for the zd_json public
 *        API.
 * @version 0.2.0
 * @date 2026-07-29
 *
 * @copyright Copyright (c) 2026
 */

#include "json_api_test.hpp"

TEST(JsonApiTest, SerializeCompactMatchesParsedInput) {
    auto document = json::Parse(R"({"a":1,"b":[true,false,null]})");
    ASSERT_TRUE(document.has_value());
    EXPECT_EQ(json::SerializeCompact(document->root()), R"({"a":1,"b":[true,false,null]})");
}

TEST(JsonApiTest, SerializePrettyIndentsNestedContainers) {
    auto document = json::Parse(R"({"a":1})");
    ASSERT_TRUE(document.has_value());

    const std::string pretty = json::SerializePretty(document->root(), 2);
    EXPECT_NE(pretty.find('\n'), std::string::npos);
    EXPECT_NE(pretty.find("  \"a\": 1"), std::string::npos);
}

TEST(JsonApiTest, SerializeDefaultsToCompact) {
    auto document = json::Parse("[1,2,3]");
    ASSERT_TRUE(document.has_value());
    EXPECT_EQ(json::Serialize(document->root()), json::SerializeCompact(document->root()));
}

TEST(JsonApiTest, SerializeHonorsExplicitOptions) {
    auto document = json::Parse(R"({"a":1})");
    ASSERT_TRUE(document.has_value());

    json::SerializeOptions options;
    options.pretty = true;
    options.indent_size = 4;
    EXPECT_EQ(json::Serialize(document->root(), options),
              json::SerializePretty(document->root(), 4));
}

TEST(JsonApiTest, ParseThenSerializeThenParseAgainIsStable) {
    constexpr auto original = R"({"name":"zuu","values":[1,2,3],"nested":{"ok":true}})";

    auto first = json::Parse(original);
    ASSERT_TRUE(first.has_value());
    const std::string serialized = json::SerializeCompact(first->root());

    auto second = json::Parse(serialized);
    ASSERT_TRUE(second.has_value());
    EXPECT_EQ(json::SerializeCompact(second->root()), serialized);
}