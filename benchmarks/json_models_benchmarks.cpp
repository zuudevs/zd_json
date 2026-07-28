/**
 * @file json_models_benchmarks.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Benchmark suite for zd_json DOM (Arena, Value, Array, Object,
 *        Document).
 * @version 0.4.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include <benchmark/benchmark.h>

#include <cstdint>
#include <string>
#include <string_view>

#include "models/arena.hpp"
#include "models/array.hpp"
#include "models/document.hpp"
#include "models/object.hpp"
#include "models/value.hpp"

namespace {

using zuu::models::Arena;
using zuu::models::Array;
using zuu::models::Document;
using zuu::models::Object;
using zuu::models::Value;

constexpr int kSmallArrayCount = 8;
constexpr int kMediumArrayCount = 128;
constexpr int kLargeArrayCount = 2048;

constexpr int kSmallObjectFieldCount = 8;
constexpr int kLargeObjectFieldCount = 64;

constexpr int kLargeArrayOfObjectsCount = 500;
constexpr double kLargeArrayValueMultiplier = 1.5;

// Mirrors the shape of kMediumJson in json_lexer_benchmarks.cpp, so DOM
// build/traversal cost can be compared against raw lexing cost for a
// realistic, moderately nested payload:
//
// {
//     "user": {
//         "id": 10482,
//         "username": "antigravity_dev",
//         "email": "dev@example.com",
//         "is_active": true,
//         "roles": ["admin", "developer", "tester"],
//         "metadata": {
//             "created_at": 1609459200,
//             "last_login": 1774500000,
//             "rating": 4.95
//         }
//     },
//     "status": "success",
//     "code": 200
// }
Value
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

// Mirrors generate_large_array_json() in json_lexer_benchmarks.cpp: an
// array of `count` small `{"index":.., "value":.., "tag":..}` objects.
// The "tag" string is generated per element (unlike the other, literal
// field names), so it must be copied into the document's arena via
// MakeString() -- otherwise the Value would point at a temporary that's
// gone by the time the caller reads it back.
Value
    BuildLargeArrayOfObjects(Document& document, int count) {
    Array::Builder array_builder;
    for (int i = 0; i < count; ++i) {
        Object::Builder object_builder;
        object_builder.Insert("index", Value{static_cast<int64_t>(i)});
        object_builder.Insert("value", Value{static_cast<double>(i) * kLargeArrayValueMultiplier});
        object_builder.Insert(
            "tag", Value{document.MakeString("item_" + std::to_string(i))});
        array_builder.Push(Value{object_builder.Build(document.arena())});
    }
    return Value{array_builder.Build(document.arena())};
}

Array*
    BuildFlatIntArray(Document& document, int count) {
    Array::Builder builder;
    for (int i = 0; i < count; ++i) {
        builder.Push(Value{static_cast<int64_t>(i)});
    }
    return builder.Build(document.arena());
}

Object*
    BuildFlatObject(Document& document, int count) {
    Object::Builder builder;
    for (int i = 0; i < count; ++i) {
        const std::string_view key = document.MakeString("field_" + std::to_string(i));
        builder.Insert(key, Value{static_cast<int64_t>(i)});
    }
    return builder.Build(document.arena());
}

// Recursively sums every numeric leaf, walking Array/Object payloads.
// Exercises the DOM's read path: type dispatch, Array/Object iteration,
// and the typed accessors.
int64_t
    SumNumbers(const Value& value) {
    switch (value.type()) {
        case zuu::enums::JsonType::Integer:
            return value.as_integer().value();
        case zuu::enums::JsonType::Float:
            return static_cast<int64_t>(value.as_float().value());
        case zuu::enums::JsonType::Array: {
            int64_t sum = 0;
            for (const Value& element : *value.as_array().value()) {
                sum += SumNumbers(element);
            }
            return sum;
        }
        case zuu::enums::JsonType::Object: {
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

} // namespace

// --- Arena: raw allocation cost -------------------------------------------

static void
    Arena_AllocateInt_SteadyState(benchmark::State& state) {
    // A single, already-warmed-up arena: measures the pure bump-pointer
    // cost once its first block has already grown large enough that no
    // further block allocations are needed.
    Arena arena;
    for (int i = 0; i < 1024; ++i) {
        benchmark::DoNotOptimize(arena.Create<int>(i));
    }

    for (auto _ : state) {
        int* value = arena.Create<int>(42);
        benchmark::DoNotOptimize(value);
        benchmark::ClobberMemory();
    }
}

static void
    New_AllocateInt_Baseline(benchmark::State& state) {
    // Baseline: one heap allocation + deallocation per element via
    // operator new/delete, for comparison against the bump allocator.
    for (auto _ : state) {
        int* value = new int(42);
        benchmark::DoNotOptimize(value);
        benchmark::ClobberMemory();
        delete value;
    }
}

static void
    Arena_ColdStart_ManySmallInts(benchmark::State& state) {
    // A fresh Arena per iteration: includes the cost of growing blocks
    // from nothing, which is representative of parsing a brand-new
    // document rather than reusing one across many parses.
    for (auto _ : state) {
        Arena arena;
        for (int i = 0; i < kMediumArrayCount; ++i) {
            benchmark::DoNotOptimize(arena.Create<int>(i));
        }
        benchmark::ClobberMemory();
    }
}

// --- Array::Builder ---------------------------------------------------------

static void
    Array_Builder_Build_Small(benchmark::State& state) {
    for (auto _ : state) {
        Document document;
        Array* array = BuildFlatIntArray(document, kSmallArrayCount);
        benchmark::DoNotOptimize(array);
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) * kSmallArrayCount);
}

static void
    Array_Builder_Build_Medium(benchmark::State& state) {
    for (auto _ : state) {
        Document document;
        Array* array = BuildFlatIntArray(document, kMediumArrayCount);
        benchmark::DoNotOptimize(array);
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) * kMediumArrayCount);
}

static void
    Array_Builder_Build_Large(benchmark::State& state) {
    for (auto _ : state) {
        Document document;
        Array* array = BuildFlatIntArray(document, kLargeArrayCount);
        benchmark::DoNotOptimize(array);
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) * kLargeArrayCount);
}

// --- Object::Builder ---------------------------------------------------------

static void
    Object_Builder_Build_Small(benchmark::State& state) {
    for (auto _ : state) {
        Document document;
        Object* object = BuildFlatObject(document, kSmallObjectFieldCount);
        benchmark::DoNotOptimize(object);
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) * kSmallObjectFieldCount);
}

static void
    Object_Builder_Build_Large(benchmark::State& state) {
    for (auto _ : state) {
        Document document;
        Object* object = BuildFlatObject(document, kLargeObjectFieldCount);
        benchmark::DoNotOptimize(object);
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) * kLargeObjectFieldCount);
}

// --- Object::Find (linear scan) ---------------------------------------------

static void
    Object_Find_Hit_Small(benchmark::State& state) {
    Document document;
    Object* object = BuildFlatObject(document, kSmallObjectFieldCount);
    for (auto _ : state) {
        auto result = object->Find("field_3");
        benchmark::DoNotOptimize(result);
    }
}

static void
    Object_Find_Hit_Large(benchmark::State& state) {
    Document document;
    Object* object = BuildFlatObject(document, kLargeObjectFieldCount);
    for (auto _ : state) {
        // Deliberately near the end, to reflect Find()'s worst case for a
        // linear scan rather than its best case.
        auto result = object->Find("field_60");
        benchmark::DoNotOptimize(result);
    }
}

static void
    Object_Find_Miss_Large(benchmark::State& state) {
    Document document;
    Object* object = BuildFlatObject(document, kLargeObjectFieldCount);
    for (auto _ : state) {
        auto result = object->Find("does_not_exist");
        benchmark::DoNotOptimize(result);
    }
}

// --- Document: realistic build & traversal ----------------------------------

static void
    Document_Fresh_MediumTree(benchmark::State& state) {
    // A brand-new Document (and thus a brand-new Arena) per iteration,
    // representative of parsing one JSON payload at a time.
    for (auto _ : state) {
        Document document;
        document.set_root(BuildMediumTree(document));
        benchmark::DoNotOptimize(document.root());
        benchmark::ClobberMemory();
    }
}

static void
    Document_Reused_MediumTree(benchmark::State& state) {
    // The same Document reused across iterations via Reset(), showing the
    // benefit of a bump allocator that can be recycled between parses
    // instead of tearing down and rebuilding its blocks every time.
    Document document;
    for (auto _ : state) {
        document.Reset();
        document.set_root(BuildMediumTree(document));
        benchmark::DoNotOptimize(document.root());
        benchmark::ClobberMemory();
    }
}

static void
    Document_LargeArrayOfObjects(benchmark::State& state) {
    for (auto _ : state) {
        Document document;
        document.set_root(BuildLargeArrayOfObjects(document, kLargeArrayOfObjectsCount));
        benchmark::DoNotOptimize(document.root());
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) * kLargeArrayOfObjectsCount);
}

static void
    Document_Traverse_MediumTree(benchmark::State& state) {
    Document document;
    document.set_root(BuildMediumTree(document));

    for (auto _ : state) {
        int64_t sum = SumNumbers(document.root());
        benchmark::DoNotOptimize(sum);
    }
}

static void
    Document_Traverse_LargeArrayOfObjects(benchmark::State& state) {
    Document document;
    document.set_root(BuildLargeArrayOfObjects(document, kLargeArrayOfObjectsCount));

    for (auto _ : state) {
        int64_t sum = SumNumbers(document.root());
        benchmark::DoNotOptimize(sum);
    }
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) * kLargeArrayOfObjectsCount);
}

BENCHMARK(Arena_AllocateInt_SteadyState);
BENCHMARK(New_AllocateInt_Baseline);
BENCHMARK(Arena_ColdStart_ManySmallInts);

BENCHMARK(Array_Builder_Build_Small);
BENCHMARK(Array_Builder_Build_Medium);
BENCHMARK(Array_Builder_Build_Large);

BENCHMARK(Object_Builder_Build_Small);
BENCHMARK(Object_Builder_Build_Large);

BENCHMARK(Object_Find_Hit_Small);
BENCHMARK(Object_Find_Hit_Large);
BENCHMARK(Object_Find_Miss_Large);

BENCHMARK(Document_Fresh_MediumTree);
BENCHMARK(Document_Reused_MediumTree);
BENCHMARK(Document_LargeArrayOfObjects);
BENCHMARK(Document_Traverse_MediumTree);
BENCHMARK(Document_Traverse_LargeArrayOfObjects);

BENCHMARK_MAIN();