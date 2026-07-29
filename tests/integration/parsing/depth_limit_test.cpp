/**
 * @file depth_limit_test.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Integration tests verifying nesting depth limits during parsing.
 * @version 0.7.0
 * @date 2026-07-29
 *
 * @copyright Copyright (c) 2026
 */

#include "json_parsing_integration_test.hpp"

namespace {

std::string
    MakeNestedArrays(int targetDepth) {
    std::string text(static_cast<size_t>(targetDepth), '[');
    text += "1";
    text.append(static_cast<size_t>(targetDepth), ']');
    return text;
}

} // namespace

TEST(JsonParsingIntegrationTest, ParseAcceptsNestingUpToDepthLimit) {
    const std::string inputPayload = MakeNestedArrays(512);

    auto document = json::Parse(inputPayload);
    ASSERT_TRUE(document.has_value());
    EXPECT_EQ(json::SerializeCompact(document->root()), inputPayload);
}

TEST(JsonParsingIntegrationTest, ParseRejectsNestingBeyondDepthLimit) {
    const std::string inputPayload = MakeNestedArrays(513);

    auto document = json::Parse(inputPayload);
    ASSERT_FALSE(document.has_value());
    EXPECT_EQ(document.error(), json::JsonErrc::DepthLimitExceeded);
}

TEST(JsonParsingIntegrationTest, ParseRejectsNestingFarBeyondDepthLimit) {
    const std::string inputPayload = MakeNestedArrays(1000);

    auto document = json::Parse(inputPayload);
    ASSERT_FALSE(document.has_value());
    EXPECT_EQ(document.error(), json::JsonErrc::DepthLimitExceeded);
}
