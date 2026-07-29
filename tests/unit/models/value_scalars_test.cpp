/**
 * @file value_scalars_test.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Unit tests for Value scalar types.
 * @version 0.7.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include "json_models_test.hpp"

TEST(JsonModelsTest, ValueScalars) {
    const Value null_value;
    EXPECT_TRUE(null_value.is_null());
    EXPECT_EQ(null_value.type(), JsonType::Null);

    const Value bool_value{true};
    EXPECT_TRUE(bool_value.is_bool());
    EXPECT_EQ(bool_value.as_bool().value(), true);
    EXPECT_FALSE(bool_value.as_integer().has_value());
    EXPECT_EQ(bool_value.as_integer().error(), JsonErrc::InvalidType);

    const Value int_value{static_cast<int64_t>(-42)};
    EXPECT_TRUE(int_value.is_integer());
    EXPECT_TRUE(int_value.is_number());
    EXPECT_EQ(int_value.as_integer().value(), -42);
    EXPECT_DOUBLE_EQ(int_value.as_double().value(), -42.0);

    const Value float_value{3.25};
    EXPECT_TRUE(float_value.is_float());
    EXPECT_TRUE(float_value.is_number());
    EXPECT_DOUBLE_EQ(float_value.as_float().value(), 3.25);
    EXPECT_DOUBLE_EQ(float_value.as_double().value(), 3.25);

    const Value string_value{std::string_view{"hi"}};
    EXPECT_TRUE(string_value.is_string());
    EXPECT_EQ(string_value.as_string().value(), "hi");
    EXPECT_FALSE(string_value.as_bool().has_value());
}
