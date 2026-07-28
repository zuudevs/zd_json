/**
 * @file parse_array_test.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Unit tests for ParseArray.
 * @version 0.7.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include "json_parser_test.hpp"

TEST(JsonParserTest, ParseArrayEmpty) {
    Arena arena;
    constexpr auto jsonPayload = "[]";
    size_t pos = 0;
    const auto parseResult = ParseArray(jsonPayload, pos, std::strlen(jsonPayload), arena, 0);
    ASSERT_TRUE(parseResult.has_value());
    EXPECT_TRUE((*parseResult)->empty());
    EXPECT_EQ(pos, std::strlen(jsonPayload));
}

TEST(JsonParserTest, ParseArrayFlatMixed) {
    Arena arena;
    constexpr auto jsonPayload = R"([1, 2.5, "three", true, false, null])";
    size_t pos = 0;
    const auto parseResult = ParseArray(jsonPayload, pos, std::strlen(jsonPayload), arena, 0);
    ASSERT_TRUE(parseResult.has_value());
    const Array* array = *parseResult;
    EXPECT_EQ(array->size(), 6u);
    EXPECT_EQ(array->at(0).value()->as_integer().value(), 1);
    EXPECT_TRUE(approx_equal(array->at(1).value()->as_double().value(), 2.5));
    EXPECT_EQ(array->at(2).value()->as_string().value(), "three");
    EXPECT_EQ(array->at(3).value()->as_bool().value(), true);
    EXPECT_EQ(array->at(4).value()->as_bool().value(), false);
    EXPECT_TRUE(array->at(5).value()->is_null());
    EXPECT_EQ(pos, std::strlen(jsonPayload));
}

TEST(JsonParserTest, ParseArrayNested) {
    Arena arena;
    constexpr auto jsonPayload = "[[1, 2], [3, [4, 5]]]";
    size_t pos = 0;
    const auto parseResult = ParseArray(jsonPayload, pos, std::strlen(jsonPayload), arena, 0);
    ASSERT_TRUE(parseResult.has_value());
    const Array* outer = *parseResult;
    EXPECT_EQ(outer->size(), 2u);

    const Array* firstArray = outer->at(0).value()->as_array().value();
    EXPECT_EQ(firstArray->size(), 2u);
    EXPECT_EQ(firstArray->at(0).value()->as_integer().value(), 1);
    EXPECT_EQ(firstArray->at(1).value()->as_integer().value(), 2);

    const Array* secondArray = outer->at(1).value()->as_array().value();
    EXPECT_EQ(secondArray->size(), 2u);
    EXPECT_EQ(secondArray->at(0).value()->as_integer().value(), 3);

    const Array* nestedArray = secondArray->at(1).value()->as_array().value();
    EXPECT_EQ(nestedArray->size(), 2u);
    EXPECT_EQ(nestedArray->at(1).value()->as_integer().value(), 5);
}

TEST(JsonParserTest, ParseArrayWhitespace) {
    Arena arena;
    constexpr auto jsonPayload = "  [ 1 ,\n\t2 , 3 ]  ";
    size_t pos = 2;
    const auto parseResult = ParseArray(jsonPayload, pos, std::strlen(jsonPayload), arena, 0);
    ASSERT_TRUE(parseResult.has_value());
    EXPECT_EQ((*parseResult)->size(), 3u);
}

TEST(JsonParserTest, ParseArrayMalformed) {
    Arena arena;

    {
        const char* strPayload = "1, 2]";
        size_t pos = 0;
        const auto parseResult = ParseArray(strPayload, pos, std::strlen(strPayload), arena, 0);
        EXPECT_FALSE(parseResult.has_value());
        EXPECT_EQ(parseResult.error(), JsonErrc::InvalidFormat);
    }
    {
        const char* strPayload = "[1, 2";
        size_t pos = 0;
        const auto parseResult = ParseArray(strPayload, pos, std::strlen(strPayload), arena, 0);
        EXPECT_FALSE(parseResult.has_value());
        EXPECT_EQ(parseResult.error(), JsonErrc::InvalidFormat);
    }
    {
        const char* strPayload = "[1, 2,]";
        size_t pos = 0;
        const auto parseResult = ParseArray(strPayload, pos, std::strlen(strPayload), arena, 0);
        EXPECT_FALSE(parseResult.has_value());
        EXPECT_EQ(parseResult.error(), JsonErrc::TrailingComma);
    }
    {
        const char* strPayload = "[1 2]";
        size_t pos = 0;
        const auto parseResult = ParseArray(strPayload, pos, std::strlen(strPayload), arena, 0);
        EXPECT_FALSE(parseResult.has_value());
        EXPECT_EQ(parseResult.error(), JsonErrc::MissingComma);
    }
    {
        const char* strPayload = "[1, ]";
        size_t pos = 0;
        const auto parseResult = ParseArray(strPayload, pos, std::strlen(strPayload), arena, 0);
        EXPECT_FALSE(parseResult.has_value());
        EXPECT_EQ(parseResult.error(), JsonErrc::TrailingComma);
    }
}
