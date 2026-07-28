/**
 * @file write_escaped_string_test.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Unit tests for WriteEscapedString.
 * @version 0.7.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include "json_serializer_test.hpp"

TEST(JsonSerializerTest, WriteEscapedString) {
    struct EscapedTestCase {
        const char* input;
        const char* expected;
    };

    const EscapedTestCase testCases[] = {
        {"", "\"\""},
        {"hello", "\"hello\""},
        {"line\nbreak", "\"line\\nbreak\""},
        {"tab\there", "\"tab\\there\""},
        {"back\bspace", "\"back\\bspace\""},
        {"form\ffeed", "\"form\\ffeed\""},
        {"carriage\rreturn", "\"carriage\\rreturn\""},
        {"quote\"inside", "\"quote\\\"inside\""},
        {"back\\slash", "\"back\\\\slash\""},
        {"slash/ok", "\"slash/ok\""},
    };
    for (const auto& testCase : testCases) {
        std::string outStr;
        WriteEscapedString(outStr, testCase.input);
        EXPECT_EQ(outStr, testCase.expected);
    }

    // Every other control character below 0x20 falls back to \u00XX.
    {
        std::string outStr;
        WriteEscapedString(outStr, std::string_view{"\x01\x02\x1F", 3});
        EXPECT_EQ(outStr, "\"\\u0001\\u0002\\u001f\"");
    }

    // Raw multi-byte UTF-8 is copied through unchanged.
    {
        std::string outStr;
        WriteEscapedString(outStr, "caf\xC3\xA9");
        EXPECT_EQ(outStr, "\"caf\xC3\xA9\"");
    }

    // A NUL byte embedded in the string content.
    {
        std::string outStr;
        WriteEscapedString(outStr, std::string_view{"a\0b", 3});
        const std::string_view expectedView{"\"a\\u0000b\"", 10};
        EXPECT_EQ(outStr, expectedView);
    }

    // Appends -- doesn't clobber -- existing buffer content.
    {
        std::string outStr = "prefix:";
        WriteEscapedString(outStr, "x");
        EXPECT_EQ(outStr, "prefix:\"x\"");
    }
}
