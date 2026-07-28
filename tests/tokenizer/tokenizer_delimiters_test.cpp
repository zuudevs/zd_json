/**
 * @file tokenizer_delimiters_test.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Unit test for Tokenize scanning delimiters and structural tokens.
 * @version 0.7.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include "json_tokenizer_test.hpp"

TEST(JsonTokenizerTest, TokenizeDelimiters) {
    constexpr auto json = R"({"name": "zuu", "age": 20, "skills": ["cpp", "go"]})";

    const auto tokens = zuu::lexer::Tokenize(json);

    ASSERT_FALSE(tokens.empty());

    // '{' -> ObjectBeg
    EXPECT_EQ(tokens[0].type, static_cast<uint64_t>(zuu::enums::TokenType::ObjectBeg));
    EXPECT_EQ(tokens[0].pos, 0);

    // ':' -> Colon
    EXPECT_EQ(tokens[1].type, static_cast<uint64_t>(zuu::enums::TokenType::Colon));
    EXPECT_EQ(tokens[1].pos, 7);

    // ',' -> Comma
    EXPECT_EQ(tokens[2].type, static_cast<uint64_t>(zuu::enums::TokenType::Comma));
    EXPECT_EQ(tokens[2].pos, 14);

    // '[' -> ArrayBeg
    EXPECT_EQ(tokens[6].type, static_cast<uint64_t>(zuu::enums::TokenType::ArrayBeg));
    EXPECT_EQ(tokens[6].pos, 37);

    // ']' -> ArrayEnd
    EXPECT_EQ(tokens[8].type, static_cast<uint64_t>(zuu::enums::TokenType::ArrayEnd));
    EXPECT_EQ(tokens[8].pos, 49);

    // '}' -> ObjectEnd
    EXPECT_EQ(tokens[9].type, static_cast<uint64_t>(zuu::enums::TokenType::ObjectEnd));
    EXPECT_EQ(tokens[9].pos, 50);
}
