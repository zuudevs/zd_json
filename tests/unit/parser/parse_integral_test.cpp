/**
 * @file parse_integral_test.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Unit tests for ParseIntegral.
 * @version 0.7.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include "json_parser_test.hpp"

TEST(JsonParserTest, ParseIntegralSwarBoundaries) {
    // Values spanning every SWAR block boundary: 1..17 digits.
    for (int numDigits = 1; numDigits <= 17; ++numDigits) {
        const std::string digitsPayload(static_cast<size_t>(numDigits), '7');
        const auto parseResult =
            ParseIntegral(digitsPayload.data(), digitsPayload.data() + digitsPayload.size());
        ASSERT_TRUE(parseResult.has_value());
        EXPECT_EQ(*parseResult, std::strtoull(digitsPayload.c_str(), nullptr, 10));
    }
}

TEST(JsonParserTest, ParseIntegralSpecialCases) {
    {
        constexpr auto strPayload = "12345678";
        const auto parseResult = ParseIntegral(strPayload, strPayload + 8);
        ASSERT_TRUE(parseResult.has_value());
        EXPECT_EQ(*parseResult, 12345678ULL);
    }
    {
        constexpr auto strPayload = "1234567890123456";
        const auto parseResult = ParseIntegral(strPayload, strPayload + 16);
        ASSERT_TRUE(parseResult.has_value());
        EXPECT_EQ(*parseResult, 1234567890123456ULL);
    }
    {
        constexpr auto strPayload = "-123456789012";
        const auto parseResult = ParseIntegral(strPayload, strPayload + 13);
        ASSERT_TRUE(parseResult.has_value());
        EXPECT_EQ(*parseResult, static_cast<uint64_t>(-123456789012LL));
    }
    {
        constexpr auto strPayload = "18446744073709551615";
        const auto parseResult = ParseIntegral(strPayload, strPayload + 20);
        ASSERT_TRUE(parseResult.has_value());
        EXPECT_EQ(*parseResult, 18446744073709551615ULL);
    }
    {
        constexpr auto strPayload = "0";
        const auto parseResult = ParseIntegral(strPayload, strPayload + 1);
        ASSERT_TRUE(parseResult.has_value());
        EXPECT_EQ(*parseResult, 0ULL);
    }
}

TEST(JsonParserTest, ParseIntegralMalformed) {
    const char* invalidInputs[] = {"", "-", "12a34", "1.5", "--5", "12 34", "1a2345678"};
    for (auto* invalidStr : invalidInputs) {
        const auto parseResult = ParseIntegral(invalidStr, invalidStr + std::strlen(invalidStr));
        EXPECT_FALSE(parseResult.has_value());
        EXPECT_EQ(parseResult.error(), JsonErrc::InvalidFormat);
    }
}
