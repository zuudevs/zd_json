/**
 * @file serialize_options_test.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Integration tests verifying explicit SerializeOptions configuration.
 * @version 0.7.0
 * @date 2026-07-29
 *
 * @copyright Copyright (c) 2026
 */

#include "json_serialization_integration_test.hpp"

TEST(JsonSerializationIntegrationTest, SerializeHonorsExplicitOptions) {
    auto document = json::Parse(R"({"a":1})");
    ASSERT_TRUE(document.has_value());

    json::SerializeOptions customOptions;
    customOptions.pretty = true;
    customOptions.indent_size = 4;
    EXPECT_EQ(json::Serialize(document->root(), customOptions),
              json::SerializePretty(document->root(), 4));
}
