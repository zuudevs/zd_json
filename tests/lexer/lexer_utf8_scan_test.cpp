/**
 * @file lexer_utf8_scan_test.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Unit tests for ScanString's UTF-8 and control character validation.
 * @version 0.7.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include "json_lexer_test.hpp"

TEST(JsonLexerTest, Utf8AndControlCharValidation) {
    // Well-formed 2/3/4-byte UTF-8 sequences.
    {
        constexpr auto strPayload = "\"caf\xC3\xA9 \xE4\xB8\xAD \xF0\x9F\x98\x80\"";
        std::string_view strView(strPayload);
        const auto scanResult = zuu::json::lexer::ScanString(strView, 0, strView.size());
        EXPECT_EQ(scanResult.error, zuu::json::JsonErrc::None);
        EXPECT_EQ(scanResult.value_end, strView.size());
    }

    // Invalid continuation byte after a 2-byte lead.
    {
        constexpr auto strPayload = "\"\xC3"
                                    "\x28"
                                    "bad\"";
        std::string_view strView(strPayload);
        const auto scanResult = zuu::json::lexer::ScanString(strView, 0, strView.size());
        EXPECT_EQ(scanResult.error, zuu::json::JsonErrc::InvalidUnicode);
    }

    // Overlong 2-byte encoding of U+0000 (0xC0 0x80).
    {
        constexpr auto strPayload = "\"\xC0\x80\"";
        std::string_view strView(strPayload);
        const auto scanResult = zuu::json::lexer::ScanString(strView, 0, strView.size());
        EXPECT_EQ(scanResult.error, zuu::json::JsonErrc::InvalidUnicode);
    }

    // A UTF-16 surrogate value (U+D800) encoded directly as UTF-8 bytes.
    {
        constexpr auto strPayload = "\"\xED\xA0\x80\"";
        std::string_view strView(strPayload);
        const auto scanResult = zuu::json::lexer::ScanString(strView, 0, strView.size());
        EXPECT_EQ(scanResult.error, zuu::json::JsonErrc::InvalidSurrogate);
    }

    // Unescaped raw control character.
    {
        std::string strPayload = "\"";
        strPayload.push_back('\x09');
        strPayload += "\"";
        std::string_view strView(strPayload);
        const auto scanResult = zuu::json::lexer::ScanString(strView, 0, strView.size());
        EXPECT_EQ(scanResult.error, zuu::json::JsonErrc::UnescapedCharacter);
    }

    // An ASCII run spanning multiple 8-byte SWAR blocks scans cleanly.
    {
        std::string strPayload = "\"" + std::string(40, 'a') + "\"";
        std::string_view strView(strPayload);
        const auto scanResult = zuu::json::lexer::ScanString(strView, 0, strView.size());
        EXPECT_EQ(scanResult.error, zuu::json::JsonErrc::None);
        EXPECT_EQ(scanResult.value_end, strView.size());
    }

    // The error is still visible through the full LexValues pipeline.
    {
        constexpr auto strPayload = "\"\xC0\x80\"";
        const auto invalidTokens = zuu::json::lexer::Tokenize(strPayload);
        const auto invalidValues = zuu::json::lexer::LexValues(strPayload, invalidTokens);
        ASSERT_EQ(invalidValues.size(), 1u);
        EXPECT_EQ(invalidValues[0].error, zuu::json::JsonErrc::InvalidUnicode);
    }
}
