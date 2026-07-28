/**
 * @file parse_float_test.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Unit tests for ParseFloat.
 * @version 0.7.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include "json_parser_test.hpp"

TEST(JsonParserTest, ParseFloatValidCases) {
    struct FloatTestCase {
        const char* input;
        double expected;
    };

    const FloatTestCase testCases[] = {
        {"3.14", 3.14},
        {"0.1", 0.1},
        {"-3.14", -3.14},
        {"1e10", 1e10},
        {"1E10", 1e10},
        {"1e-10", 1e-10},
        {"1.5e3", 1500.0},
        {"-1.5e-3", -0.0015},
        {"0", 0.0},
        {"0.0", 0.0},
        {"123456789.123456", 123456789.123456},
        {"2.2250738585072014e-308", 2.2250738585072014e-308},
    };
    for (const auto& testCase : testCases) {
        const auto parseResult =
            ParseFloat(testCase.input, testCase.input + std::strlen(testCase.input));
        ASSERT_TRUE(parseResult.has_value());
        EXPECT_TRUE(approx_equal(*parseResult, testCase.expected));
    }
}

TEST(JsonParserTest, ParseFloatMalformed) {
    const char* invalidInputs[] = {
        "", "-", ".", "-.", "1e", "1e+", "1e-", "1.2.3", "e5", "1.2x3", "1x"};
    for (auto* invalidStr : invalidInputs) {
        const auto parseResult = ParseFloat(invalidStr, invalidStr + std::strlen(invalidStr));
        EXPECT_FALSE(parseResult.has_value());
        EXPECT_EQ(parseResult.error(), JsonErrc::InvalidFormat);
    }
}
