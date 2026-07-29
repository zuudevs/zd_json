/**
 * @file serialize_scalars_test.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Unit tests for scalar Value serialization.
 * @version 0.7.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include "json_serializer_test.hpp"

TEST(JsonSerializerTest, SerializeScalars) {
    EXPECT_EQ(SerializeCompact(Value{}), "null");
    EXPECT_EQ(SerializeCompact(Value{nullptr}), "null");
    EXPECT_EQ(SerializeCompact(Value{true}), "true");
    EXPECT_EQ(SerializeCompact(Value{false}), "false");
    EXPECT_EQ(SerializeCompact(Value{static_cast<int64_t>(42)}), "42");
    EXPECT_EQ(SerializeCompact(Value{static_cast<int64_t>(-7)}), "-7");
    EXPECT_EQ(SerializeCompact(Value{3.5}), "3.5");
    EXPECT_EQ(SerializeCompact(Value{std::string_view{"hi"}}), "\"hi\"");
    EXPECT_EQ(SerializeCompact(Value{std::string_view{}}), "\"\"");
}
