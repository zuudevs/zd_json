/**
 * @file serialize_parse_round_trip_test.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Unit tests for parse -> serialize -> parse round-trip verification.
 * @version 0.7.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include "json_serializer_test.hpp"

TEST(JsonSerializerTest, RoundTripParseSerialize) {
    const char* sampleInputs[] = {
        "0",
        "-1",
        "3.14",
        "1.5e10",
        "-2.2250738585072014e-308",
        "\"\"",
        "\"hello world\"",
        "\"line\\nbreak\\ttab\"",
        "[]",
        "{}",
        "[1,2,3]",
        "[1,[2,[3,[4,[5]]]]]",
        R"({"nested":{"a":{"b":{"c":1}}}})",
        R"({"values":[1,2,3],"nested":{"ok":true,"inner":{"deep":1}}})",
        "true",
        "false",
        "null",
    };

    for (const char* sampleStr : sampleInputs) {
        const std::string_view jsonView{sampleStr};

        Arena arena1;
        size_t readPos1 = 0;
        const auto parsedResult1 = ParseValue(jsonView, readPos1, jsonView.size(), arena1, 0);
        ASSERT_TRUE(parsedResult1.has_value());

        // Compact form re-parses to the same compact form.
        const std::string compactStr = SerializeCompact(*parsedResult1);
        Arena arena2;
        size_t readPos2 = 0;
        const auto parsedResult2 = ParseValue(compactStr, readPos2, compactStr.size(), arena2, 0);
        ASSERT_TRUE(parsedResult2.has_value());
        EXPECT_EQ(SerializeCompact(*parsedResult2), compactStr);

        // Pretty form re-parses to the exact same compact form too.
        const std::string prettyStr = SerializePretty(*parsedResult1);
        Arena arena3;
        size_t readPos3 = 0;
        const auto parsedResult3 = ParseValue(prettyStr, readPos3, prettyStr.size(), arena3, 0);
        ASSERT_TRUE(parsedResult3.has_value());
        EXPECT_EQ(SerializeCompact(*parsedResult3), compactStr);
    }
}
