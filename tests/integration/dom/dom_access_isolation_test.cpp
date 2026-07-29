/**
 * @file dom_access_isolation_test.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Integration tests verifying DOM errors do not corrupt sibling nodes.
 * @version 0.7.0
 * @date 2026-07-29
 *
 * @copyright Copyright (c) 2026
 */

#include "json_dom_integration_test.hpp"

TEST(JsonDomIntegrationTest, DomErrorsDoNotPreventFurtherValidAccess) {
    auto document = json::Parse(R"({"a":1,"b":[1,2,3]})");
    ASSERT_TRUE(document.has_value());

    const json::Object* object = document->root().as_object().value();
    EXPECT_FALSE(object->Find("missing").has_value());
    EXPECT_EQ(object->Find("a").value()->as_integer().value(), 1);

    const json::Array* array = object->Find("b").value()->as_array().value();
    EXPECT_FALSE(array->at(10).has_value());
    EXPECT_EQ(array->at(2).value()->as_integer().value(), 3);
}
