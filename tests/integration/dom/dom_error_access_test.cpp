/**
 * @file dom_error_access_test.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Integration tests verifying DOM type-mismatch and boundary errors.
 * @version 0.7.0
 * @date 2026-07-29
 *
 * @copyright Copyright (c) 2026
 */

#include "json_dom_integration_test.hpp"

TEST(JsonDomIntegrationTest, AsArrayOnNonArrayValueFailsWithIsNotArray) {
    auto document = json::Parse("42");
    ASSERT_TRUE(document.has_value());

    auto arrayResult = document->root().as_array();
    ASSERT_FALSE(arrayResult.has_value());
    EXPECT_EQ(arrayResult.error(), json::JsonErrc::IsNotArray);
}

TEST(JsonDomIntegrationTest, AsObjectOnNonObjectValueFailsWithIsNotObject) {
    auto document = json::Parse(R"("just a string")");
    ASSERT_TRUE(document.has_value());

    auto objectResult = document->root().as_object();
    ASSERT_FALSE(objectResult.has_value());
    EXPECT_EQ(objectResult.error(), json::JsonErrc::IsNotObject);
}

TEST(JsonDomIntegrationTest, ArrayAtOutOfBoundsIndexFailsWithOutOfBound) {
    auto document = json::Parse("[1,2,3]");
    ASSERT_TRUE(document.has_value());

    const json::Array* array = document->root().as_array().value();
    auto elementResult = array->at(10);
    ASSERT_FALSE(elementResult.has_value());
    EXPECT_EQ(elementResult.error(), json::JsonErrc::OutOfBound);
}

TEST(JsonDomIntegrationTest, ObjectFindMissingKeyFailsWithOutOfBound) {
    auto document = json::Parse(R"({"a":1})");
    ASSERT_TRUE(document.has_value());

    const json::Object* object = document->root().as_object().value();
    auto memberResult = object->Find("missing");
    ASSERT_FALSE(memberResult.has_value());
    EXPECT_EQ(memberResult.error(), json::JsonErrc::OutOfBound);
}
