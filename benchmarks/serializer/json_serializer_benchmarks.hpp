/**
 * @file json_serializer_benchmarks.hpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Benchmark function declarations and shared payload constants/helpers for zd_json
 * serializer benchmarks.
 * @version 0.7.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#pragma once

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

void
    WriteInteger_Short(benchmark::State& state);
void
    WriteInteger_Long(benchmark::State& state);
void
    WriteInteger_Negative(benchmark::State& state);
void
    WriteInteger_Min(benchmark::State& state);

void
    WriteFloat_Simple(benchmark::State& state);
void
    WriteFloat_Exponent(benchmark::State& state);
void
    WriteFloat_WholeNumber(benchmark::State& state);
void
    WriteFloat_NonFinite(benchmark::State& state);

void
    WriteEscapedString_PlainShort(benchmark::State& state);
void
    WriteEscapedString_PlainLong(benchmark::State& state);
void
    WriteEscapedString_CommonEscapes(benchmark::State& state);
void
    WriteEscapedString_ControlEscapes(benchmark::State& state);
void
    WriteEscapedString_MultibyteUtf8Passthrough(benchmark::State& state);

void
    Serialize_FlatIntArray_Small_Compact(benchmark::State& state);
void
    Serialize_FlatIntArray_Large_Compact(benchmark::State& state);
void
    Serialize_FlatIntArray_Large_Pretty(benchmark::State& state);

void
    Serialize_MediumTree_Compact(benchmark::State& state);
void
    Serialize_MediumTree_Pretty2(benchmark::State& state);
void
    Serialize_MediumTree_Pretty4(benchmark::State& state);

void
    Serialize_LargeArrayOfObjects_Compact(benchmark::State& state);
void
    Serialize_LargeArrayOfObjects_Pretty(benchmark::State& state);

namespace constants {

using zuu::json::models::Arena;
using zuu::json::models::Array;
using zuu::json::models::Document;
using zuu::json::models::Object;
using zuu::json::models::Value;
using zuu::json::serializer::Serialize;
using zuu::json::serializer::SerializeCompact;
using zuu::json::serializer::SerializeOptions;
using zuu::json::serializer::SerializePretty;
using zuu::json::serializer::WriteEscapedString;
using zuu::json::serializer::WriteFloat;
using zuu::json::serializer::WriteInteger;

constexpr int64_t kShortInt = 42;
constexpr int64_t kLongInt = 1234567890123456789;
constexpr int64_t kNegativeInt = -987654321012345;
constexpr int64_t kMinInt = std::numeric_limits<int64_t>::min();

constexpr double kSimpleFloat = 3.14;
constexpr double kExponentFloat = 6.02214076e23;
constexpr double kWholeFloat = 100.0;
constexpr double kNaN = std::numeric_limits<double>::quiet_NaN();

constexpr std::string_view kPlainShortString = "hello";
constexpr std::string_view kPlainLongString =
    "this_is_a_much_longer_string_with_no_escapes_at_all_just_ascii_text";

constexpr std::string_view kCommonEscapesString = "line1\nline2\ttabbed\\backslash\"quoted";

constexpr std::string_view kControlEscapesString = "col1\x01"
                                                   "col2\x1f"
                                                   "col3";

constexpr std::string_view kMultibyteUtf8String =
    "caf\xC3\xA9 \xE4\xB8\xAD\xE6\x96\x87 \xF0\x9F\x98\x80";

constexpr int kSmallArrayCount = 8;
constexpr int kLargeArrayCount = 2048;
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

} // namespace constants
