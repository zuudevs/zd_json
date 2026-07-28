/**
 * @file lexer_long_escaped_string_test.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Unit tests for long string and escaped string lexing.
 * @version 0.7.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include "json_lexer_test.hpp"

TEST(JsonLexerTest, LongAndEscapedStrings) {
    constexpr auto jsonLongPayload =
        R"({"escaped": "hello\nworld", "long": "this_is_a_very_long_string_exceeding_sixteen_chars"})";
    const auto tokensLong = zuu::lexer::Tokenize(jsonLongPayload);
    const auto valuesLong = zuu::lexer::LexValues(jsonLongPayload, tokensLong);

    ASSERT_GE(valuesLong.size(), 5u);

    // "hello\nworld" has escape -> String
    EXPECT_EQ(valuesLong[2].type, zuu::enums::JsonType::String);
    EXPECT_EQ(valuesLong[2].value, R"("hello\nworld")");

    // long string > 16 chars -> String
    EXPECT_EQ(valuesLong[4].type, zuu::enums::JsonType::String);

    for (const auto& valItem : valuesLong) {
        EXPECT_EQ(valItem.error, zuu::JsonErrc::None);
    }
}
