/**
 * @file document_tree_test.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Unit tests for Document tree manipulation and reset.
 * @version 0.7.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include "json_models_test.hpp"

TEST(JsonModelsTest, DocumentTree) {
    // Builds the equivalent of:
    // {"name": "zuu", "values": [1, 2, 3], "nested": {"ok": true}}
    Document document;

    Array::Builder values_builder;
    values_builder.Push(Value{static_cast<int64_t>(1)});
    values_builder.Push(Value{static_cast<int64_t>(2)});
    values_builder.Push(Value{static_cast<int64_t>(3)});
    Array* values = values_builder.Build(document.arena());

    Object::Builder nested_builder;
    nested_builder.Insert("ok", Value{true});
    Object* nested = nested_builder.Build(document.arena());

    Object::Builder root_builder;
    root_builder.Insert("name", Value{document.MakeString("zuu")});
    root_builder.Insert("values", Value{values});
    root_builder.Insert("nested", Value{nested});
    Object* root = root_builder.Build(document.arena());

    document.set_root(Value{root});

    EXPECT_TRUE(document.root().is_object());
    const Object* result = document.root().as_object().value();

    EXPECT_EQ(result->Find("name").value()->as_string().value(), "zuu");

    const Array* result_values = result->Find("values").value()->as_array().value();
    EXPECT_EQ(result_values->size(), 3u);
    EXPECT_EQ(result_values->at(1).value()->as_integer().value(), 2);

    const Object* result_nested = result->Find("nested").value()->as_object().value();
    EXPECT_EQ(result_nested->Find("ok").value()->as_bool().value(), true);

    // Reset() must invalidate the tree and bring the document back to a
    // clean, reusable null-root state.
    document.Reset();
    EXPECT_TRUE(document.root().is_null());
    EXPECT_EQ(document.arena().bytes_allocated(), 0u);
}
