/**
 * @file parse_null_test.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Unit tests for ParseLiteral (null).
 * @version 0.7.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include "json_parser_test.hpp"

TEST(JsonParserTest, ParseNullValid) {
    constexpr auto strPayload = "null";
    const auto parseResult = ParseLiteral(strPayload, strPayload + 4);
    ASSERT_TRUE(parseResult.has_value());
    EXPECT_EQ(*parseResult, -1);
}

TEST(JsonParserTest, ParseNullMalformed) {
    const char* invalidInputs[] = {"", "nul", "Null", "nulll", "NULL"};
    for (auto* invalidStr : invalidInputs) {
        const auto parseResult = ParseLiteral(invalidStr, invalidStr + std::strlen(invalidStr));
        EXPECT_FALSE(parseResult.has_value());
        EXPECT_EQ(parseResult.error(), JsonErrc::InvalidValue);
    }
}
