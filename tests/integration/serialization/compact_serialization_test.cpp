/**
 * @file compact_serialization_test.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Integration tests verifying SerializeCompact output format.
 * @version 0.7.0
 * @date 2026-07-29
 *
 * @copyright Copyright (c) 2026
 */

#include "json_serialization_integration_test.hpp"

TEST(JsonSerializationIntegrationTest, SerializeCompactMatchesParsedInput) {
    auto document = json::Parse(R"({"a":1,"b":[true,false,null]})");
    ASSERT_TRUE(document.has_value());
    EXPECT_EQ(json::SerializeCompact(document->root()), R"({"a":1,"b":[true,false,null]})");
}

TEST(JsonSerializationIntegrationTest, SerializeDefaultsToCompact) {
    auto document = json::Parse("[1,2,3]");
    ASSERT_TRUE(document.has_value());
    EXPECT_EQ(json::Serialize(document->root()), json::SerializeCompact(document->root()));
}
