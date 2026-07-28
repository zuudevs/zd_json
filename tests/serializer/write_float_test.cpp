/**
 * @file write_float_test.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Unit tests for WriteFloat.
 * @version 0.7.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include "json_serializer_test.hpp"

TEST(JsonSerializerTest, WriteFloat) {
    // Values whose to_chars rendering already contains '.', 'e', or 'E'
    {
        std::string outStr;
        WriteFloat(outStr, 3.14);
        EXPECT_EQ(outStr, "3.14");
    }
    {
        std::string outStr;
        WriteFloat(outStr, 0.1);
        EXPECT_EQ(outStr, "0.1");
    }
    {
        std::string outStr;
        WriteFloat(outStr, 1.5e10);
        EXPECT_EQ(outStr, "1.5e+10");
    }
    {
        std::string outStr;
        WriteFloat(outStr, -3.25);
        EXPECT_EQ(outStr, "-3.25");
    }

    // Values that render as a bare integer from to_chars
    {
        std::string outStr;
        WriteFloat(outStr, 3.0);
        EXPECT_EQ(outStr, "3.0");
    }
    {
        std::string outStr;
        WriteFloat(outStr, 0.0);
        EXPECT_EQ(outStr, "0.0");
    }
    {
        std::string outStr;
        WriteFloat(outStr, -0.0);
        EXPECT_EQ(outStr, "-0.0");
    }
    {
        std::string outStr;
        WriteFloat(outStr, 100.0);
        EXPECT_EQ(outStr, "100.0");
    }

    // Non-finite values fall back to `null`.
    {
        std::string outStr;
        WriteFloat(outStr, std::numeric_limits<double>::quiet_NaN());
        EXPECT_EQ(outStr, "null");
    }
    {
        std::string outStr;
        WriteFloat(outStr, std::numeric_limits<double>::infinity());
        EXPECT_EQ(outStr, "null");
    }
    {
        std::string outStr;
        WriteFloat(outStr, -std::numeric_limits<double>::infinity());
        EXPECT_EQ(outStr, "null");
    }

    // Extreme-magnitude doubles must fit comfortably.
    {
        std::string outStr;
        WriteFloat(outStr, 2.2250738585072014e-308);
        EXPECT_NE(outStr.find("2.2250738585072014e-308"), std::string::npos);
    }
    {
        std::string outStr;
        WriteFloat(outStr, std::numeric_limits<double>::max());
        EXPECT_FALSE(outStr.empty());
        double parsedValue = 0.0;
        std::from_chars(outStr.data(), outStr.data() + outStr.size(), parsedValue);
        EXPECT_DOUBLE_EQ(parsedValue, std::numeric_limits<double>::max());
    }

    // Appends -- doesn't clobber -- existing buffer content.
    {
        std::string outStr = "prefix:";
        WriteFloat(outStr, 1.5);
        EXPECT_EQ(outStr, "prefix:1.5");
    }
}
