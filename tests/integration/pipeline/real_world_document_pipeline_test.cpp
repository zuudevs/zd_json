/**
 * @file real_world_document_pipeline_test.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Integration tests parsing a realistic, mixed-type JSON document.
 * @version 0.7.0
 * @date 2026-07-29
 *
 * @copyright Copyright (c) 2026
 */

#include "json_pipeline_integration_test.hpp"

namespace {

constexpr auto kOriginalDocument =
    R"({"id":1024,"name":"caf\u00e9 Ada","tags":["admin","beta-tester"],)"
    R"("rating":4.5,"negative":-273.15,"scientific":6.022e23,)"
    R"("description":"line1\nline2\ttabbed \"quoted\" back\\slash",)"
    R"("emoji":"\uD83D\uDE00","active":true,"deleted":false,"parent":null,)"
    R"("scores":[10,-5,0,3.5],"nested":{"level":1,"children":[{"level":2,"children":[]}]}})";

} // namespace

TEST(JsonPipelineIntegrationTest, RealWorldDocumentParsesEveryFieldCorrectly) {
    auto document = json::Parse(kOriginalDocument);
    ASSERT_TRUE(document.has_value());

    const json::Object* root = document->root().as_object().value();

    EXPECT_EQ(root->Find("id").value()->as_integer().value(), 1024);
    EXPECT_EQ(root->Find("name").value()->as_string().value(), "caf\xC3\xA9 Ada");
    EXPECT_EQ(root->Find("emoji").value()->as_string().value(), "\xF0\x9F\x98\x80");
    EXPECT_TRUE(root->Find("parent").value()->is_null());
    EXPECT_TRUE(root->Find("active").value()->as_bool().value());
    EXPECT_FALSE(root->Find("deleted").value()->as_bool().value());
    EXPECT_DOUBLE_EQ(root->Find("rating").value()->as_double().value(), 4.5);
    EXPECT_DOUBLE_EQ(root->Find("negative").value()->as_double().value(), -273.15);
    EXPECT_DOUBLE_EQ(root->Find("scientific").value()->as_double().value(), 6.022e23);

    const json::Array* tags = root->Find("tags").value()->as_array().value();
    ASSERT_EQ(tags->size(), 2U);
    EXPECT_EQ(tags->at(0).value()->as_string().value(), "admin");

    const json::Array* scores = root->Find("scores").value()->as_array().value();
    ASSERT_EQ(scores->size(), 4U);
    EXPECT_EQ(scores->at(1).value()->as_integer().value(), -5);
    EXPECT_DOUBLE_EQ(scores->at(3).value()->as_double().value(), 3.5);

    const json::Object* nested = root->Find("nested").value()->as_object().value();
    EXPECT_EQ(nested->Find("level").value()->as_integer().value(), 1);

    const json::Array* children = nested->Find("children").value()->as_array().value();
    ASSERT_EQ(children->size(), 1U);

    const json::Object* grandchild = children->at(0).value()->as_object().value();
    EXPECT_TRUE(grandchild->Find("children").value()->as_array().value()->empty());
}
