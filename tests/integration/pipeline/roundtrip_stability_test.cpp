/**
 * @file roundtrip_stability_test.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Integration tests verifying round-trip stability across parse -> serialize cycles.
 * @version 0.7.0
 * @date 2026-07-29
 *
 * @copyright Copyright (c) 2026
 */

#include "json_pipeline_integration_test.hpp"

namespace {

constexpr auto kOriginalDocument =
    R"({"id":1024,"name":"caf\u00e9 Ada","tags":["admin","beta-tester"],)"
    R"("rating":4.5,"negative":-273.15,"scientific":6.022e23,)"
    R"("description":"line1\nline2\ttabbed \"quoted\" back\\slash",)"
    R"("emoji":"\uD83D\uDE00","active":true,"deleted":false,"parent":null,)"
    R"("scores":[10,-5,0,3.5],"nested":{"level":1,"children":[{"level":2,"children":[]}]}})";

} // namespace

TEST(JsonPipelineIntegrationTest, ParseThenSerializeThenParseAgainIsStable) {
    constexpr auto originalJson = R"({"name":"zuu","values":[1,2,3],"nested":{"ok":true}})";

    auto firstDoc = json::Parse(originalJson);
    ASSERT_TRUE(firstDoc.has_value());
    const std::string serializedText = json::SerializeCompact(firstDoc->root());

    auto secondDoc = json::Parse(serializedText);
    ASSERT_TRUE(secondDoc.has_value());
    EXPECT_EQ(json::SerializeCompact(secondDoc->root()), serializedText);
}

TEST(JsonPipelineIntegrationTest, RealWorldDocumentSurvivesRepeatedSerializeParseCycles) {
    auto firstDoc = json::Parse(kOriginalDocument);
    ASSERT_TRUE(firstDoc.has_value());
    const std::string firstSerialized = json::SerializeCompact(firstDoc->root());

    auto secondDoc = json::Parse(firstSerialized);
    ASSERT_TRUE(secondDoc.has_value());
    const std::string secondSerialized = json::SerializeCompact(secondDoc->root());

    EXPECT_EQ(firstSerialized, secondSerialized);
}

TEST(JsonPipelineIntegrationTest, RealWorldDocumentPrettyPrintReparsesToSameData) {
    auto document = json::Parse(kOriginalDocument);
    ASSERT_TRUE(document.has_value());

    const std::string prettyOutput = json::SerializePretty(document->root(), 4);

    auto reparsedDoc = json::Parse(prettyOutput);
    ASSERT_TRUE(reparsedDoc.has_value());

    EXPECT_EQ(json::SerializeCompact(reparsedDoc->root()),
              json::SerializeCompact(document->root()));
}

TEST(JsonPipelineIntegrationTest, SerializePrettyPreservesStructureThroughRoundTrip) {
    constexpr auto originalJson = R"({"a":1,"b":[1,2,{"c":true}],"d":{},"e":[]})";

    auto firstDoc = json::Parse(originalJson);
    ASSERT_TRUE(firstDoc.has_value());

    const std::string prettyOutput = json::SerializePretty(firstDoc->root(), 2);

    auto secondDoc = json::Parse(prettyOutput);
    ASSERT_TRUE(secondDoc.has_value());
    EXPECT_EQ(json::SerializeCompact(secondDoc->root()), originalJson);
}
