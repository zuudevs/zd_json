/**
 * @file large_document_parsing_test.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Integration tests parsing large documents requiring arena block expansion.
 * @version 0.7.0
 * @date 2026-07-29
 *
 * @copyright Copyright (c) 2026
 */

#include "json_parsing_integration_test.hpp"

namespace {

json::ParseOptions
    SmallBlockOptions() {
    json::ParseOptions options;
    options.initial_block_size = 64;
    return options;
}

} // namespace

TEST(JsonParsingIntegrationTest, ParsesLargeArrayAcrossArenaBlocks) {
    std::string inputPayload = "[";
    for (int elemIndex = 0; elemIndex < 5000; ++elemIndex) {
        if (elemIndex != 0) {
            inputPayload += ",";
        }
        inputPayload += std::to_string(elemIndex);
    }
    inputPayload += "]";

    auto document = json::Parse(inputPayload, SmallBlockOptions());
    ASSERT_TRUE(document.has_value());

    const json::Array* array = document->root().as_array().value();
    ASSERT_EQ(array->size(), 5000U);
    EXPECT_EQ(array->at(0).value()->as_integer().value(), 0);
    EXPECT_EQ(array->at(4999).value()->as_integer().value(), 4999);
}

TEST(JsonParsingIntegrationTest, ParsesLargeObjectAcrossArenaBlocks) {
    std::string inputPayload = "{";
    for (int elemIndex = 0; elemIndex < 2000; ++elemIndex) {
        if (elemIndex != 0) {
            inputPayload += ",";
        }
        inputPayload += "\"k" + std::to_string(elemIndex) + "\":" + std::to_string(elemIndex);
    }
    inputPayload += "}";

    auto document = json::Parse(inputPayload, SmallBlockOptions());
    ASSERT_TRUE(document.has_value());

    const json::Object* object = document->root().as_object().value();
    ASSERT_EQ(object->size(), 2000U);
    EXPECT_EQ(object->Find("k0").value()->as_integer().value(), 0);
    EXPECT_EQ(object->Find("k1999").value()->as_integer().value(), 1999);
}
