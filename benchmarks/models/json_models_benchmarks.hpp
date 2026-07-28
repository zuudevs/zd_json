/**
 * @file json_models_benchmarks.hpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Benchmark function declarations and shared DOM helpers for zd_json models benchmarks.
 * @version 0.7.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#pragma once

#include <benchmark/benchmark.h>
#include <cstdint>
#include <string>
#include <string_view>

#include "models/arena.hpp"
#include "models/array.hpp"
#include "models/document.hpp"
#include "models/object.hpp"
#include "models/value.hpp"

void
    Arena_AllocateInt_SteadyState(benchmark::State& state);
void
    New_AllocateInt_Baseline(benchmark::State& state);
void
    Arena_ColdStart_ManySmallInts(benchmark::State& state);

void
    Array_Builder_Build_Small(benchmark::State& state);
void
    Array_Builder_Build_Medium(benchmark::State& state);
void
    Array_Builder_Build_Large(benchmark::State& state);

void
    Object_Builder_Build_Small(benchmark::State& state);
void
    Object_Builder_Build_Large(benchmark::State& state);

void
    Object_Find_Hit_Small(benchmark::State& state);
void
    Object_Find_Hit_Large(benchmark::State& state);
void
    Object_Find_Miss_Large(benchmark::State& state);

void
    Document_Fresh_MediumTree(benchmark::State& state);
void
    Document_Reused_MediumTree(benchmark::State& state);
void
    Document_LargeArrayOfObjects(benchmark::State& state);
void
    Document_Traverse_MediumTree(benchmark::State& state);
void
    Document_Traverse_LargeArrayOfObjects(benchmark::State& state);

namespace constants {

using zuu::json::models::Arena;
using zuu::json::models::Array;
using zuu::json::models::Document;
using zuu::json::models::Object;
using zuu::json::models::Value;

constexpr int kSmallArrayCount = 8;
constexpr int kMediumArrayCount = 128;
constexpr int kLargeArrayCount = 2048;

constexpr int kSmallObjectFieldCount = 8;
constexpr int kLargeObjectFieldCount = 64;

constexpr int kLargeArrayOfObjectsCount = 500;
constexpr double kLargeArrayValueMultiplier = 1.5;

inline Value
    BuildMediumTree(Document& document) {
    Array::Builder roles_builder;
    roles_builder.Push(Value{std::string_view{"admin"}});
    roles_builder.Push(Value{std::string_view{"developer"}});
    roles_builder.Push(Value{std::string_view{"tester"}});
    Array* roles = roles_builder.Build(document.arena());

    Object::Builder metadata_builder;
    metadata_builder.Insert("created_at", Value{static_cast<int64_t>(1609459200)});
    metadata_builder.Insert("last_login", Value{static_cast<int64_t>(1774500000)});
    metadata_builder.Insert("rating", Value{4.95});
    Object* metadata = metadata_builder.Build(document.arena());

    Object::Builder user_builder;
    user_builder.Insert("id", Value{static_cast<int64_t>(10482)});
    user_builder.Insert("username", Value{std::string_view{"antigravity_dev"}});
    user_builder.Insert("email", Value{std::string_view{"dev@example.com"}});
    user_builder.Insert("is_active", Value{true});
    user_builder.Insert("roles", Value{roles});
    user_builder.Insert("metadata", Value{metadata});
    Object* user = user_builder.Build(document.arena());

    Object::Builder root_builder;
    root_builder.Insert("user", Value{user});
    root_builder.Insert("status", Value{std::string_view{"success"}});
    root_builder.Insert("code", Value{static_cast<int64_t>(200)});
    Object* root = root_builder.Build(document.arena());

    return Value{root};
}

inline Value
    BuildLargeArrayOfObjects(Document& document, int count) {
    Array::Builder array_builder;
    for (int i = 0; i < count; ++i) {
        Object::Builder object_builder;
        object_builder.Insert("index", Value{static_cast<int64_t>(i)});
        object_builder.Insert("value", Value{static_cast<double>(i) * kLargeArrayValueMultiplier});
        object_builder.Insert("tag", Value{document.MakeString("item_" + std::to_string(i))});
        array_builder.Push(Value{object_builder.Build(document.arena())});
    }
    return Value{array_builder.Build(document.arena())};
}

inline Array*
    BuildFlatIntArray(Document& document, int count) {
    Array::Builder builder;
    for (int i = 0; i < count; ++i) {
        builder.Push(Value{static_cast<int64_t>(i)});
    }
    return builder.Build(document.arena());
}

inline Object*
    BuildFlatObject(Document& document, int count) {
    Object::Builder builder;
    for (int i = 0; i < count; ++i) {
        const std::string_view key = document.MakeString("field_" + std::to_string(i));
        builder.Insert(key, Value{static_cast<int64_t>(i)});
    }
    return builder.Build(document.arena());
}

inline int64_t
    SumNumbers(const Value& value) {
    switch (value.type()) {
        case zuu::json::enums::JsonType::Integer:
            return value.as_integer().value();
        case zuu::json::enums::JsonType::Float:
            return static_cast<int64_t>(value.as_float().value());
        case zuu::json::enums::JsonType::Array: {
            int64_t sum = 0;
            for (const Value& element : *value.as_array().value()) {
                sum += SumNumbers(element);
            }
            return sum;
        }
        case zuu::json::enums::JsonType::Object: {
            int64_t sum = 0;
            for (const auto& member : *value.as_object().value()) {
                sum += SumNumbers(member.value);
            }
            return sum;
        }
        default:
            return 0;
    }
}

} // namespace constants
