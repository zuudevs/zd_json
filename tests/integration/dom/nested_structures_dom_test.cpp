/**
 * @file nested_structures_dom_test.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Integration tests verifying DOM navigation across complex nested structures.
 * @version 0.7.0
 * @date 2026-07-29
 *
 * @copyright Copyright (c) 2026
 */

#include "json_dom_integration_test.hpp"

TEST(JsonDomIntegrationTest, ParseHandlesMixedArrayAndObjectNesting) {
    constexpr auto jsonPayload =
        R"({"users":[{"name":"ada","roles":["admin","editor"],"meta":{"active":true,)"
        R"("score":97.5}},{"name":"grace","roles":[],"meta":{"active":false,"score":0}}],)"
        R"("count":2})";

    auto document = json::Parse(jsonPayload);
    ASSERT_TRUE(document.has_value());

    const json::Value& rootValue = document->root();
    ASSERT_TRUE(rootValue.is_object());

    const json::Array* usersArray =
        rootValue.as_object().value()->Find("users").value()->as_array().value();
    ASSERT_EQ(usersArray->size(), 2U);

    const json::Object* adaObject = usersArray->at(0).value()->as_object().value();
    EXPECT_EQ(adaObject->Find("name").value()->as_string().value(), "ada");

    const json::Object* adaMeta = adaObject->Find("meta").value()->as_object().value();
    EXPECT_TRUE(adaMeta->Find("active").value()->as_bool().value());
}
