/**
 * @file lexer_types_test.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Unit tests for LexValues classification of fundamental JSON types.
 * @version 0.7.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include "json_lexer_test.hpp"

TEST(JsonLexerTest, BasicTypesClassification) {
    constexpr auto jsonPayload =
        R"({"name": "zuu", "age": 20, "score": 98.5, "active": true, "extra": null, "skills": ["cpp", "go"]})";

    const auto tokens = zuu::json::lexer::Tokenize(jsonPayload);
    const auto values = zuu::json::lexer::LexValues(jsonPayload, tokens);

    ASSERT_FALSE(values.empty());

    // 0: Object '{'
    EXPECT_EQ(values[0].type, zuu::json::enums::JsonType::Object);
    EXPECT_EQ(values[0].value, "{");

    // 1: Key "name" -> Short
    EXPECT_EQ(values[1].type, zuu::json::enums::JsonType::Short);
    EXPECT_EQ(values[1].value, R"("name")");

    // 2: Value "zuu" -> Short
    EXPECT_EQ(values[2].type, zuu::json::enums::JsonType::Short);
    EXPECT_EQ(values[2].value, R"("zuu")");

    // 3: Key "age" -> Short
    EXPECT_EQ(values[3].type, zuu::json::enums::JsonType::Short);
    EXPECT_EQ(values[3].value, R"("age")");

    // 4: Value 20 -> Integer
    EXPECT_EQ(values[4].type, zuu::json::enums::JsonType::Integer);
    EXPECT_EQ(values[4].value, "20");

    // 5: Key "score" -> Short
    EXPECT_EQ(values[5].type, zuu::json::enums::JsonType::Short);
    EXPECT_EQ(values[5].value, R"("score")");

    // 6: Value 98.5 -> Float
    EXPECT_EQ(values[6].type, zuu::json::enums::JsonType::Float);
    EXPECT_EQ(values[6].value, "98.5");

    // 7: Key "active" -> Short
    EXPECT_EQ(values[7].type, zuu::json::enums::JsonType::Short);
    EXPECT_EQ(values[7].value, R"("active")");

    // 8: Value true -> Bool
    EXPECT_EQ(values[8].type, zuu::json::enums::JsonType::Bool);
    EXPECT_EQ(values[8].value, "true");

    // 9: Key "extra" -> Short
    EXPECT_EQ(values[9].type, zuu::json::enums::JsonType::Short);
    EXPECT_EQ(values[9].value, R"("extra")");

    // 10: Value null -> Null
    EXPECT_EQ(values[10].type, zuu::json::enums::JsonType::Null);
    EXPECT_EQ(values[10].value, "null");

    // 11: Key "skills" -> Short
    EXPECT_EQ(values[11].type, zuu::json::enums::JsonType::Short);
    EXPECT_EQ(values[11].value, R"("skills")");

    // 12: Array '[' -> Array
    EXPECT_EQ(values[12].type, zuu::json::enums::JsonType::Array);
    EXPECT_EQ(values[12].value, "[");

    // 13: "cpp" -> Short
    EXPECT_EQ(values[13].type, zuu::json::enums::JsonType::Short);
    EXPECT_EQ(values[13].value, R"("cpp")");

    // 14: "go" -> Short
    EXPECT_EQ(values[14].type, zuu::json::enums::JsonType::Short);
    EXPECT_EQ(values[14].value, R"("go")");

    for (const auto& valItem : values) {
        EXPECT_EQ(valItem.error, zuu::json::JsonErrc::None);
    }
}
