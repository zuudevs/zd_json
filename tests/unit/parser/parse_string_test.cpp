/**
 * @file parse_string_test.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Unit tests for ParseShortString and ParseString.
 * @version 0.7.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include "json_parser_test.hpp"

TEST(JsonParserTest, ParseShortStringValid) {
    {
        constexpr auto strPayload = R"("hello")";
        const auto parseResult = ParseShortString(strPayload, strPayload + std::strlen(strPayload));
        ASSERT_TRUE(parseResult.has_value());
        EXPECT_EQ(*parseResult, "hello");
    }
    {
        constexpr auto strPayload = R"("")";
        const auto parseResult = ParseShortString(strPayload, strPayload + 2);
        ASSERT_TRUE(parseResult.has_value());
        EXPECT_TRUE(parseResult->empty());
    }
    {
        constexpr auto strPayload = "hello"; // missing quotes
        const auto parseResult = ParseShortString(strPayload, strPayload + 5);
        EXPECT_FALSE(parseResult.has_value());
        EXPECT_EQ(parseResult.error(), JsonErrc::InvalidFormat);
    }
}

TEST(JsonParserTest, ParseStringEscapes) {
    struct StringTestCase {
        const char* input;
        const char* expected;
    };
    const StringTestCase testCases[] = {
        {R"("hello")", "hello"},
        {R"("")", ""},
        {R"("line\nbreak")", "line\nbreak"},
        {R"("tab\there")", "tab\there"},
        {R"("quote\"inside")", "quote\"inside"},
        {R"("back\\slash")", "back\\slash"},
        {R"("slash\/ok")", "slash/ok"},
        {R"("\u0041\u0042\u0043")", "ABC"},
        {R"("emoji:\uD83D\uDE00")", "emoji:\xF0\x9F\x98\x80"},
    };
    for (const auto& testCase : testCases) {
        const auto parseResult =
            ParseString(testCase.input, testCase.input + std::strlen(testCase.input));
        ASSERT_TRUE(parseResult.has_value());
        EXPECT_EQ(*parseResult, testCase.expected);
    }
}

TEST(JsonParserTest, ParseStringMultibyteUtf8) {
    constexpr auto strPayload = "\"caf\xC3\xA9\"";
    const auto parseResult = ParseString(strPayload, strPayload + std::strlen(strPayload));
    ASSERT_TRUE(parseResult.has_value());
    EXPECT_EQ(*parseResult, "caf\xC3\xA9");
}

TEST(JsonParserTest, ParseStringBadCases) {
    struct BadStringCase {
        const char* input;
        JsonErrc expectedError;
    };
    const BadStringCase badCases[] = {
        {"hello", JsonErrc::InvalidFormat},
        {R"("bad\x")", JsonErrc::InvalidValue},
        {R"("bad\u12")", JsonErrc::InvalidValue},
        {R"("bad\uZZZZ")", JsonErrc::InvalidValue},
        {R"("\uD83Dnotlow")", JsonErrc::InvalidSurrogate},
        {R"("\uDE00")", JsonErrc::InvalidSurrogate},
    };
    for (const auto& badCase : badCases) {
        const auto parseResult =
            ParseString(badCase.input, badCase.input + std::strlen(badCase.input));
        EXPECT_FALSE(parseResult.has_value());
        EXPECT_EQ(parseResult.error(), badCase.expectedError);
    }
}

TEST(JsonParserTest, ParseStringUnescapedControl) {
    std::string strPayload = "\"bad";
    strPayload.push_back('\x01');
    strPayload += "char\"";
    const auto parseResult =
        ParseString(strPayload.c_str(), strPayload.c_str() + strPayload.size());
    EXPECT_FALSE(parseResult.has_value());
    EXPECT_EQ(parseResult.error(), JsonErrc::UnescapedCharacter);
}
