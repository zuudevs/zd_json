/**
 * @file write_integer_test.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Unit tests for WriteInteger.
 * @version 0.7.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include "json_serializer_test.hpp"

TEST(JsonSerializerTest, WriteInteger) {
    struct IntegerTestCase {
        int64_t input;
        const char* expected;
    };

    const IntegerTestCase testCases[] = {
        {0, "0"},
        {1, "1"},
        {-1, "-1"},
        {42, "42"},
        {-42, "-42"},
        {123456789, "123456789"},
        {std::numeric_limits<int64_t>::max(), "9223372036854775807"},
        {std::numeric_limits<int64_t>::min(), "-9223372036854775808"},
    };

    for (const auto& testCase : testCases) {
        std::string outStr;
        WriteInteger(outStr, testCase.input);
        EXPECT_EQ(outStr, testCase.expected);
    }

    // Appends -- doesn't clobber -- existing buffer content.
    {
        std::string outStr = "prefix:";
        WriteInteger(outStr, 7);
        EXPECT_EQ(outStr, "prefix:7");
    }
}
