/**
 * @file json_serializer_benchmarks.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Benchmark suite for WriteInteger, WriteFloat, WriteEscapedString,
 *        and Serialize/SerializeCompact/SerializePretty.
 * @version 0.6.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include <benchmark/benchmark.h>

#include <cstdint>
#include <limits>
#include <string>
#include <string_view>

#include "models/arena.hpp"
#include "models/array.hpp"
#include "models/document.hpp"
#include "models/object.hpp"
#include "models/value.hpp"
#include "serializer/serialize_number.hpp"
#include "serializer/serialize_options.hpp"
#include "serializer/serialize_string.hpp"
#include "serializer/serializer.hpp"

namespace {

using zuu::models::Arena;
using zuu::models::Array;
using zuu::models::Document;
using zuu::models::Object;
using zuu::models::Value;
using zuu::serializer::Serialize;
using zuu::serializer::SerializeCompact;
using zuu::serializer::SerializeOptions;
using zuu::serializer::SerializePretty;
using zuu::serializer::WriteEscapedString;
using zuu::serializer::WriteFloat;
using zuu::serializer::WriteInteger;

constexpr int64_t kShortInt = 42;
constexpr int64_t kLongInt = 1234567890123456789;
constexpr int64_t kNegativeInt = -987654321012345;
constexpr int64_t kMinInt = std::numeric_limits<int64_t>::min();

constexpr double kSimpleFloat = 3.14;
constexpr double kExponentFloat = 6.02214076e23;
// Renders as a bare integer from to_chars ("100"), forcing WriteFloat's
// trailing ".0" path so the text still round-trips as JsonType::Float.
constexpr double kWholeFloat = 100.0;
constexpr double kNaN = std::numeric_limits<double>::quiet_NaN();

// No characters JSON requires escaping: exercises the passthrough path.
constexpr std::string_view kPlainShortString = "hello";
constexpr std::string_view kPlainLongString =
    "this_is_a_much_longer_string_with_no_escapes_at_all_just_ascii_text";

// A realistic mix of the common two-character escapes.
constexpr std::string_view kCommonEscapesString = "line1\nline2\ttabbed\\backslash\"quoted";

// Control characters below 0x20 with no short escape, forcing the
// \u00XX fallback.
constexpr std::string_view kControlEscapesString = "col1\x01"
                                                   "col2\x1f"
                                                   "col3";

// Raw multi-byte UTF-8, copied through unchanged (mirrors the parser's
// treatment of already-valid UTF-8).
constexpr std::string_view kMultibyteUtf8String =
    "caf\xC3\xA9 \xE4\xB8\xAD\xE6\x96\x87 \xF0\x9F\x98\x80";

constexpr int kSmallArrayCount = 8;
constexpr int kLargeArrayCount = 2048;
constexpr int kLargeArrayOfObjectsCount = 500;
constexpr double kLargeArrayValueMultiplier = 1.5;

// Mirrors BuildMediumTree() in json_models_benchmarks.cpp, so serialization
// cost can be compared against DOM build/traversal cost for the same
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

// Mirrors BuildLargeArrayOfObjects() in json_models_benchmarks.cpp: an
// array of `count` small {"index":.., "value":.., "tag":..} objects, so
// serializing a large, flat-ish payload can be compared against the cost
// of building/traversing the same shape.
Value
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

Array*
    BuildFlatIntArray(Document& document, int count) {
    Array::Builder builder;
    for (int i = 0; i < count; ++i) {
        builder.Push(Value{static_cast<int64_t>(i)});
    }
    return builder.Build(document.arena());
}

} // namespace

// --- WriteInteger ------------------------------------------------------

static void
    WriteInteger_Short(benchmark::State& state) {
    for (auto _ : state) {
        std::string out;
        WriteInteger(out, kShortInt);
        benchmark::DoNotOptimize(out);
    }
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
}

static void
    WriteInteger_Long(benchmark::State& state) {
    for (auto _ : state) {
        std::string out;
        WriteInteger(out, kLongInt);
        benchmark::DoNotOptimize(out);
    }
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
}

static void
    WriteInteger_Negative(benchmark::State& state) {
    for (auto _ : state) {
        std::string out;
        WriteInteger(out, kNegativeInt);
        benchmark::DoNotOptimize(out);
    }
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
}

// Exercises the int64_t::min edge case, which can't be negated in place
// the way an ordinary negative value can.
static void
    WriteInteger_Min(benchmark::State& state) {
    for (auto _ : state) {
        std::string out;
        WriteInteger(out, kMinInt);
        benchmark::DoNotOptimize(out);
    }
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
}

// --- WriteFloat ----------------------------------------------------------

static void
    WriteFloat_Simple(benchmark::State& state) {
    for (auto _ : state) {
        std::string out;
        WriteFloat(out, kSimpleFloat);
        benchmark::DoNotOptimize(out);
    }
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
}

static void
    WriteFloat_Exponent(benchmark::State& state) {
    for (auto _ : state) {
        std::string out;
        WriteFloat(out, kExponentFloat);
        benchmark::DoNotOptimize(out);
    }
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
}

// Forces the trailing ".0" append path (to_chars produced neither '.'
// nor 'e'/'E').
static void
    WriteFloat_WholeNumber(benchmark::State& state) {
    for (auto _ : state) {
        std::string out;
        WriteFloat(out, kWholeFloat);
        benchmark::DoNotOptimize(out);
    }
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
}

// Non-finite fallback: writes the literal "null" instead of calling
// std::to_chars at all.
static void
    WriteFloat_NonFinite(benchmark::State& state) {
    for (auto _ : state) {
        std::string out;
        WriteFloat(out, kNaN);
        benchmark::DoNotOptimize(out);
    }
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
}

// --- WriteEscapedString ----------------------------------------------------

// No escapes needed: the whole content is copied through unchanged, aside
// from the surrounding quotes.
static void
    WriteEscapedString_PlainShort(benchmark::State& state) {
    for (auto _ : state) {
        std::string out;
        WriteEscapedString(out, kPlainShortString);
        benchmark::DoNotOptimize(out);
    }
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
}

static void
    WriteEscapedString_PlainLong(benchmark::State& state) {
    for (auto _ : state) {
        std::string out;
        WriteEscapedString(out, kPlainLongString);
        benchmark::DoNotOptimize(out);
    }
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
}

// A realistic mix of \n, \t, \\, and \" two-character escapes.
static void
    WriteEscapedString_CommonEscapes(benchmark::State& state) {
    for (auto _ : state) {
        std::string out;
        WriteEscapedString(out, kCommonEscapesString);
        benchmark::DoNotOptimize(out);
    }
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
}

// Control bytes below 0x20 with no short form, forcing the \u00XX
// fallback for every one of them.
static void
    WriteEscapedString_ControlEscapes(benchmark::State& state) {
    for (auto _ : state) {
        std::string out;
        WriteEscapedString(out, kControlEscapesString);
        benchmark::DoNotOptimize(out);
    }
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
}

// Raw multi-byte UTF-8, left untouched byte-for-byte.
static void
    WriteEscapedString_MultibyteUtf8Passthrough(benchmark::State& state) {
    for (auto _ : state) {
        std::string out;
        WriteEscapedString(out, kMultibyteUtf8String);
        benchmark::DoNotOptimize(out);
    }
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
}

// --- Serialize: flat containers -------------------------------------------

static void
    Serialize_FlatIntArray_Small_Compact(benchmark::State& state) {
    Document document;
    Value value{BuildFlatIntArray(document, kSmallArrayCount)};

    for (auto _ : state) {
        std::string out = SerializeCompact(value);
        benchmark::DoNotOptimize(out);
    }
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) * kSmallArrayCount);
}

static void
    Serialize_FlatIntArray_Large_Compact(benchmark::State& state) {
    Document document;
    Value value{BuildFlatIntArray(document, kLargeArrayCount)};

    for (auto _ : state) {
        std::string out = SerializeCompact(value);
        benchmark::DoNotOptimize(out);
    }
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) * kLargeArrayCount);
}

static void
    Serialize_FlatIntArray_Large_Pretty(benchmark::State& state) {
    Document document;
    Value value{BuildFlatIntArray(document, kLargeArrayCount)};

    for (auto _ : state) {
        std::string out = SerializePretty(value);
        benchmark::DoNotOptimize(out);
    }
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) * kLargeArrayCount);
}

// --- Serialize: realistic nested tree --------------------------------------

static void
    Serialize_MediumTree_Compact(benchmark::State& state) {
    Document document;
    Value value = BuildMediumTree(document);

    for (auto _ : state) {
        std::string out = SerializeCompact(value);
        benchmark::DoNotOptimize(out);
    }
    state.SetBytesProcessed(static_cast<int64_t>(state.iterations()) *
                            static_cast<int64_t>(SerializeCompact(value).size()));
}

static void
    Serialize_MediumTree_Pretty2(benchmark::State& state) {
    Document document;
    Value value = BuildMediumTree(document);

    for (auto _ : state) {
        std::string out = SerializePretty(value, 2);
        benchmark::DoNotOptimize(out);
    }
    state.SetBytesProcessed(static_cast<int64_t>(state.iterations()) *
                            static_cast<int64_t>(SerializePretty(value, 2).size()));
}

// Wider indentation only changes how much whitespace gets appended per
// nesting level, so this quantifies that marginal cost against Pretty2.
static void
    Serialize_MediumTree_Pretty4(benchmark::State& state) {
    Document document;
    Value value = BuildMediumTree(document);

    for (auto _ : state) {
        std::string out = SerializePretty(value, 4);
        benchmark::DoNotOptimize(out);
    }
    state.SetBytesProcessed(static_cast<int64_t>(state.iterations()) *
                            static_cast<int64_t>(SerializePretty(value, 4).size()));
}

// --- Serialize: large array of small objects -------------------------------

static void
    Serialize_LargeArrayOfObjects_Compact(benchmark::State& state) {
    Document document;
    Value value = BuildLargeArrayOfObjects(document, kLargeArrayOfObjectsCount);

    for (auto _ : state) {
        std::string out = SerializeCompact(value);
        benchmark::DoNotOptimize(out);
    }
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) * kLargeArrayOfObjectsCount);
}

static void
    Serialize_LargeArrayOfObjects_Pretty(benchmark::State& state) {
    Document document;
    Value value = BuildLargeArrayOfObjects(document, kLargeArrayOfObjectsCount);

    for (auto _ : state) {
        std::string out = SerializePretty(value);
        benchmark::DoNotOptimize(out);
    }
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) * kLargeArrayOfObjectsCount);
}

BENCHMARK(WriteInteger_Short);
BENCHMARK(WriteInteger_Long);
BENCHMARK(WriteInteger_Negative);
BENCHMARK(WriteInteger_Min);

BENCHMARK(WriteFloat_Simple);
BENCHMARK(WriteFloat_Exponent);
BENCHMARK(WriteFloat_WholeNumber);
BENCHMARK(WriteFloat_NonFinite);

BENCHMARK(WriteEscapedString_PlainShort);
BENCHMARK(WriteEscapedString_PlainLong);
BENCHMARK(WriteEscapedString_CommonEscapes);
BENCHMARK(WriteEscapedString_ControlEscapes);
BENCHMARK(WriteEscapedString_MultibyteUtf8Passthrough);

BENCHMARK(Serialize_FlatIntArray_Small_Compact);
BENCHMARK(Serialize_FlatIntArray_Large_Compact);
BENCHMARK(Serialize_FlatIntArray_Large_Pretty);

BENCHMARK(Serialize_MediumTree_Compact);
BENCHMARK(Serialize_MediumTree_Pretty2);
BENCHMARK(Serialize_MediumTree_Pretty4);

BENCHMARK(Serialize_LargeArrayOfObjects_Compact);
BENCHMARK(Serialize_LargeArrayOfObjects_Pretty);

BENCHMARK_MAIN();