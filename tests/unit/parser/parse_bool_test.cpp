/**
 * @file parse_bool_test.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Unit tests for ParseLiteral (boolean).
 * @version 0.7.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include "json_parser_test.hpp"

TEST(JsonParserTest, ParseBoolTrue) {
    constexpr auto strPayload = "true";
    const auto parseResult = ParseLiteral(strPayload, strPayload + 4);
    ASSERT_TRUE(parseResult.has_value());
    EXPECT_TRUE(static_cast<bool>(*parseResult));
}

TEST(JsonParserTest, ParseBoolFalse) {
    constexpr auto strPayload = "false";
    const auto parseResult = ParseLiteral(strPayload, strPayload + 5);
    ASSERT_TRUE(parseResult.has_value());
    EXPECT_FALSE(static_cast<bool>(*parseResult));
}

TEST(JsonParserTest, ParseBoolMalformed) {
    const char* invalidInputs[] = {"", "tru", "True", "falze", "truee", "fals", "nulll"};
    for (auto* invalidStr : invalidInputs) {
        const auto parseResult = ParseLiteral(invalidStr, invalidStr + std::strlen(invalidStr));
        EXPECT_FALSE(parseResult.has_value());
        EXPECT_EQ(parseResult.error(), JsonErrc::InvalidValue);
    }
}
