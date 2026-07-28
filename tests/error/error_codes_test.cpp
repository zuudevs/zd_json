/**
 * @file error_codes_test.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Unit tests for JsonErrc string conversions.
 * @version 0.7.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include "json_error_test.hpp"

TEST(JsonErrorTest, StringConversions) {
    using zuu::JsonErrc;
    using zuu::to_string;

    EXPECT_STREQ(to_string(JsonErrc::None), "No error");
    EXPECT_STREQ(to_string(JsonErrc::CommentNotAllowed),
                 "Comments are not allowed in standard JSON");
    EXPECT_STREQ(to_string(JsonErrc::DepthLimitExceeded), "Maximum nesting depth limit exceeded");
    EXPECT_STREQ(to_string(JsonErrc::EmptyValue), "Empty JSON input or value");
    EXPECT_STREQ(to_string(JsonErrc::InvalidBooleanLiteral), "Invalid boolean literal");
    EXPECT_STREQ(to_string(JsonErrc::InvalidFormat), "Invalid JSON format");
    EXPECT_STREQ(to_string(JsonErrc::InvalidNullLiteral), "Invalid null literal");
    EXPECT_STREQ(to_string(JsonErrc::InvalidSurrogate), "Invalid UTF-16 surrogate pair");
    EXPECT_STREQ(to_string(JsonErrc::InvalidType), "Invalid or unexpected JSON type");
    EXPECT_STREQ(to_string(JsonErrc::InvalidUnicode), "Invalid UTF-8/Unicode sequence");
    EXPECT_STREQ(to_string(JsonErrc::InvalidValue), "Invalid JSON value");
    EXPECT_STREQ(to_string(JsonErrc::IsNotArray), "JSON element is not an array");
    EXPECT_STREQ(to_string(JsonErrc::IsNotObject), "JSON element is not an object");
    EXPECT_STREQ(to_string(JsonErrc::LeadingZero), "Numbers cannot have leading zeros");
    EXPECT_STREQ(to_string(JsonErrc::MissingComma), "Missing expected comma separator");
    EXPECT_STREQ(to_string(JsonErrc::OutOfBound), "Index or value out of bounds");
    EXPECT_STREQ(to_string(JsonErrc::RootNotArrayType), "Root JSON element is not an array");
    EXPECT_STREQ(to_string(JsonErrc::RootNotObjectType), "Root JSON element is not an object");
    EXPECT_STREQ(to_string(JsonErrc::SingleQuotedString), "Strings must use double quotes");
    EXPECT_STREQ(to_string(JsonErrc::TrailingComma), "Trailing commas are not allowed");
    EXPECT_STREQ(to_string(JsonErrc::UnescapedCharacter), "Unescaped control character in string");
    EXPECT_STREQ(to_string(JsonErrc::UnquotedKey), "Object keys must be double-quoted strings");
    EXPECT_STREQ(to_string(JsonErrc::Unknown), "Unknown JSON error");
    EXPECT_STREQ(to_string(static_cast<JsonErrc>(255)), "Unknown JSON error");
}
