/**
 * @file error_propagation_test.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Integration tests verifying error code propagation through public json::Parse().
 * @version 0.7.0
 * @date 2026-07-29
 *
 * @copyright Copyright (c) 2026
 */

#include "json_parsing_integration_test.hpp"

namespace {

struct ParseErrorCase {
    const char* label;
    const char* input;
    json::JsonErrc expectedError;
};

} // namespace

TEST(JsonParsingIntegrationTest, ParseSurfacesUnderlyingErrorCodes) {
    const ParseErrorCase testCases[] = {
        {"unquoted object key", R"({a:1})", json::JsonErrc::UnquotedKey},
        {"single-quoted string", R"({'a':1})", json::JsonErrc::SingleQuotedString},
        {"trailing comma in array", "[1,2,]", json::JsonErrc::TrailingComma},
        {"missing comma in array", "[1 2]", json::JsonErrc::MissingComma},
        {"trailing comma in object", R"({"a":1,})", json::JsonErrc::TrailingComma},
        {"missing comma in object", R"({"a":1 "b":2})", json::JsonErrc::MissingComma},
        {"short unicode escape", R"("bad\u12")", json::JsonErrc::InvalidValue},
        {"non-hex unicode escape", R"("bad\uZZZZ")", json::JsonErrc::InvalidValue},
        {"unknown escape character", R"("bad\x")", json::JsonErrc::InvalidValue},
        {"lone high surrogate", R"("\uD83Dnotlow")", json::JsonErrc::InvalidSurrogate},
        {"lone low surrogate", R"("\uDE00")", json::JsonErrc::InvalidSurrogate},
        {"empty input", "", json::JsonErrc::EmptyValue},
        {"whitespace-only input", "   \n\t  ", json::JsonErrc::EmptyValue},
        {"trailing garbage after scalar", "123 abc", json::JsonErrc::InvalidFormat},
        {"trailing garbage after container", "[1,2] [3]", json::JsonErrc::InvalidFormat},
        {"unterminated object", R"({"key": 1)", json::JsonErrc::InvalidFormat},
        {"unterminated array", "[1, 2", json::JsonErrc::InvalidFormat},
        {"missing colon after key", R"({"key" 1})", json::JsonErrc::InvalidFormat},
    };

    for (const auto& testCase : testCases) {
        auto document = json::Parse(testCase.input);
        ASSERT_FALSE(document.has_value()) << testCase.label;
        EXPECT_EQ(document.error(), testCase.expectedError) << testCase.label;
    }
}

TEST(JsonParsingIntegrationTest, ParseSurfacesUnescapedControlCharacterError) {
    std::string inputPayload = "\"bad";
    inputPayload.push_back('\x01');
    inputPayload += "char\"";

    auto document = json::Parse(inputPayload);
    ASSERT_FALSE(document.has_value());
    EXPECT_EQ(document.error(), json::JsonErrc::UnescapedCharacter);
}

TEST(JsonParsingIntegrationTest, ParseErrorsDoNotProduceDocument) {
    auto document = json::Parse(R"({"key": })");
    ASSERT_FALSE(document.has_value());
}

TEST(JsonParsingIntegrationTest, ParseNestedContainerPropagatesInnermostError) {
    auto document = json::Parse(R"([1, [2, [3, {"key": }]]])");
    ASSERT_FALSE(document.has_value());
    EXPECT_EQ(document.error(), json::JsonErrc::InvalidValue);
}
