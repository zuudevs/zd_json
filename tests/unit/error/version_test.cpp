/**
 * @file version_test.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Unit tests for library version constants.
 * @version 0.7.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include "json_error_test.hpp"

TEST(JsonVersionTest, VersionConstants) {
    EXPECT_STREQ(zuu::json::zd_json_version, "0.8.0");
    EXPECT_EQ(zuu::json::zd_json_version_major, 0);
    EXPECT_EQ(zuu::json::zd_json_version_minor, 8);
    EXPECT_EQ(zuu::json::zd_json_version_patch, 0);
}
