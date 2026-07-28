/**
 * @file json_models_test.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Test suite for Arena, Value, Array, Object, and Document.
 * @version 0.4.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include <cassert>
#include <cstring>
#include <iostream>
#include <string_view>

#include "enums/json_type.hpp"
#include "models/arena.hpp"
#include "models/array.hpp"
#include "models/document.hpp"
#include "models/object.hpp"
#include "models/value.hpp"
#include "zd_json/json_error.hpp"

using zuu::JsonErrc;
using zuu::enums::JsonType;
using zuu::models::Arena;
using zuu::models::Array;
using zuu::models::Document;
using zuu::models::Object;
using zuu::models::Value;

namespace {

void
    test_arena_basic() {
    Arena arena;

    int* a = arena.Create<int>(42);
    assert(a != nullptr);
    assert(*a == 42);

    double* b = arena.Create<double>(3.5);
    assert(b != nullptr);
    assert(*b == 3.5);

    // Distinct allocations must never alias.
    assert(static_cast<void*>(a) != static_cast<void*>(b));

    const std::string_view copied = arena.CreateString("hello arena");
    assert(copied == "hello arena");
    // The string must have been copied, not merely referenced.
    assert(copied.data() != std::string_view{"hello arena"}.data());

    assert(arena.bytes_allocated() > 0);
    assert(arena.block_count() >= 1);

    std::cout << "test_arena_basic passed\n";
}

void
    test_arena_grows_across_blocks() {
    // Force at least one extra block by requesting more than a single
    // default-sized block can hold.
    Arena arena(64);

    for (int i = 0; i < 1000; ++i) {
        int* value = arena.Create<int>(i);
        assert(value != nullptr);
        assert(*value == i);
    }

    assert(arena.block_count() > 1);

    std::cout << "test_arena_grows_across_blocks passed\n";
}

void
    test_arena_reset() {
    Arena arena;
    (void)arena.Create<int>(1);
    (void)arena.Create<int>(2);

    assert(arena.bytes_allocated() > 0);

    arena.Reset();

    assert(arena.bytes_allocated() == 0);
    assert(arena.block_count() == 0);

    // The arena must still be usable after Reset().
    int* value = arena.Create<int>(7);
    assert(value != nullptr);
    assert(*value == 7);

    std::cout << "test_arena_reset passed\n";
}

void
    test_value_scalars() {
    const Value null_value;
    assert(null_value.is_null());
    assert(null_value.type() == JsonType::Null);

    const Value bool_value{true};
    assert(bool_value.is_bool());
    assert(bool_value.as_bool().value() == true);
    assert(!bool_value.as_integer().has_value());
    assert(bool_value.as_integer().error() == JsonErrc::InvalidType);

    const Value int_value{static_cast<int64_t>(-42)};
    assert(int_value.is_integer());
    assert(int_value.is_number());
    assert(int_value.as_integer().value() == -42);
    assert(int_value.as_double().value() == -42.0);

    const Value float_value{3.25};
    assert(float_value.is_float());
    assert(float_value.is_number());
    assert(float_value.as_float().value() == 3.25);
    assert(float_value.as_double().value() == 3.25);

    const Value string_value{std::string_view{"hi"}};
    assert(string_value.is_string());
    assert(string_value.as_string().value() == "hi");
    assert(!string_value.as_bool().has_value());

    std::cout << "test_value_scalars passed\n";
}

void
    test_array_builder() {
    Arena arena;

    Array::Builder builder;
    builder.Push(Value{static_cast<int64_t>(1)});
    builder.Push(Value{static_cast<int64_t>(2)});
    builder.Push(Value{static_cast<int64_t>(3)});
    assert(builder.size() == 3);

    Array* array = builder.Build(arena);
    assert(array != nullptr);
    assert(array->size() == 3);
    assert(!array->empty());

    assert(array->at(0).value()->as_integer().value() == 1);
    assert(array->at(1).value()->as_integer().value() == 2);
    assert(array->at(2).value()->as_integer().value() == 3);

    assert(!array->at(3).has_value());
    assert(array->at(3).error() == JsonErrc::OutOfBound);

    int64_t sum = 0;
    for (const Value& element : *array) {
        sum += element.as_integer().value();
    }
    assert(sum == 6);

    // An empty array is a valid, well-formed Array.
    Array::Builder empty_builder;
    Array* empty_array = empty_builder.Build(arena);
    assert(empty_array != nullptr);
    assert(empty_array->empty());
    assert(empty_array->size() == 0);

    std::cout << "test_array_builder passed\n";
}

void
    test_object_builder() {
    Arena arena;

    Object::Builder builder;
    builder.Insert("name", Value{std::string_view{"zuu"}});
    builder.Insert("age", Value{static_cast<int64_t>(20)});
    builder.Insert("active", Value{true});
    assert(builder.size() == 3);

    Object* object = builder.Build(arena);
    assert(object != nullptr);
    assert(object->size() == 3);

    assert(object->Contains("name"));
    assert(object->Find("name").value()->as_string().value() == "zuu");
    assert(object->Find("age").value()->as_integer().value() == 20);
    assert(object->Find("active").value()->as_bool().value() == true);

    assert(!object->Contains("missing"));
    assert(!object->Find("missing").has_value());
    assert(object->Find("missing").error() == JsonErrc::OutOfBound);

    std::cout << "test_object_builder passed\n";
}

void
    test_document_tree() {
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

    assert(document.root().is_object());
    const Object* result = document.root().as_object().value();

    assert(result->Find("name").value()->as_string().value() == "zuu");

    const Array* result_values = result->Find("values").value()->as_array().value();
    assert(result_values->size() == 3);
    assert(result_values->at(1).value()->as_integer().value() == 2);

    const Object* result_nested = result->Find("nested").value()->as_object().value();
    assert(result_nested->Find("ok").value()->as_bool().value() == true);

    // Reset() must invalidate the tree and bring the document back to a
    // clean, reusable null-root state.
    document.Reset();
    assert(document.root().is_null());
    assert(document.arena().bytes_allocated() == 0);

    std::cout << "test_document_tree passed\n";
}

} // namespace

int
    main() {
    test_arena_basic();
    test_arena_grows_across_blocks();
    test_arena_reset();
    test_value_scalars();
    test_array_builder();
    test_object_builder();
    test_document_tree();

    std::cout << "All models tests passed successfully!\n";
    return 0;
}
