/**
 * @file pretty_serialization_test.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Integration tests verifying SerializePretty formatting and indents.
 * @version 0.7.0
 * @date 2026-07-29
 *
 * @copyright Copyright (c) 2026
 */

#include "json_serialization_integration_test.hpp"

TEST(JsonSerializationIntegrationTest, SerializePrettyIndentsNestedContainers) {
    auto document = json::Parse(R"({"a":1})");
    ASSERT_TRUE(document.has_value());

    const std::string prettyOutput = json::SerializePretty(document->root(), 2);
    EXPECT_NE(prettyOutput.find('\n'), std::string::npos);
    EXPECT_NE(prettyOutput.find("  \"a\": 1"), std::string::npos);
}

TEST(JsonSerializationIntegrationTest,
     SerializePrettyRendersEmptyContainersWithoutInnerWhitespace) {
    auto document = json::Parse(R"({"empty_object":{},"empty_array":[]})");
    ASSERT_TRUE(document.has_value());

    const std::string prettyOutput = json::SerializePretty(document->root(), 2);
    EXPECT_NE(prettyOutput.find("\"empty_object\": {}"), std::string::npos);
    EXPECT_NE(prettyOutput.find("\"empty_array\": []"), std::string::npos);
}
