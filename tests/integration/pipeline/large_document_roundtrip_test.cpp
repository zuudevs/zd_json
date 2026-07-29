/**
 * @file large_document_roundtrip_test.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Integration tests verifying round-trip serialization on large data structures.
 * @version 0.7.0
 * @date 2026-07-29
 *
 * @copyright Copyright (c) 2026
 */

#include "json_pipeline_integration_test.hpp"

namespace {

json::ParseOptions
    SmallBlockOptions() {
    json::ParseOptions options;
    options.initial_block_size = 64;
    return options;
}

} // namespace

TEST(JsonPipelineIntegrationTest, LargeArrayRoundTripMatchesOriginalPayload) {
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

    EXPECT_EQ(json::SerializeCompact(document->root()), inputPayload);
}

TEST(JsonPipelineIntegrationTest, LargeObjectRoundTripMatchesOriginalPayload) {
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

    EXPECT_EQ(json::SerializeCompact(document->root()), inputPayload);
}

TEST(JsonPipelineIntegrationTest, LargeArrayOfObjectsPreservesEveryMemberThroughRoundTrip) {
    std::string inputPayload = "[";
    for (int elemIndex = 0; elemIndex < 500; ++elemIndex) {
        if (elemIndex != 0) {
            inputPayload += ",";
        }
        inputPayload += "{\"id\":" + std::to_string(elemIndex) +
                        ",\"active\":" + (elemIndex % 2 == 0 ? "true" : "false") + "}";
    }
    inputPayload += "]";

    auto document = json::Parse(inputPayload, SmallBlockOptions());
    ASSERT_TRUE(document.has_value());

    const json::Array* array = document->root().as_array().value();
    ASSERT_EQ(array->size(), 500U);

    const json::Object* firstObj = array->at(0).value()->as_object().value();
    EXPECT_EQ(firstObj->Find("id").value()->as_integer().value(), 0);
    EXPECT_TRUE(firstObj->Find("active").value()->as_bool().value());

    const json::Object* lastObj = array->at(499).value()->as_object().value();
    EXPECT_EQ(lastObj->Find("id").value()->as_integer().value(), 499);
    EXPECT_FALSE(lastObj->Find("active").value()->as_bool().value());

    EXPECT_EQ(json::SerializeCompact(document->root()), inputPayload);
}
