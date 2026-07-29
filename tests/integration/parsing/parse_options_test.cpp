/**
 * @file parse_options_test.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Integration tests verifying custom ParseOptions behavior.
 * @version 0.7.0
 * @date 2026-07-29
 *
 * @copyright Copyright (c) 2026
 */

#include "json_parsing_integration_test.hpp"

TEST(JsonParsingIntegrationTest, ParseRespectsInitialBlockSizeOption) {
    json::ParseOptions customOptions;
    customOptions.initial_block_size = 64;

    auto document = json::Parse(R"({"a": 1})", customOptions);
    ASSERT_TRUE(document.has_value());
    EXPECT_EQ(document->root().as_object().value()->Find("a").value()->as_integer().value(), 1);
}
