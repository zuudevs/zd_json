/**
 * @file json_serializer_test.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Test suite for WriteInteger, WriteFloat, WriteEscapedString, and
 *        Serialize/SerializeCompact/SerializePretty.
 * @version 0.6.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include <cassert>
#include <charconv>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>

#include "models/arena.hpp"
#include "models/array.hpp"
#include "models/document.hpp"
#include "models/object.hpp"
#include "models/value.hpp"
#include "parser/parse_value.hpp"
#include "serializer/serialize_number.hpp"
#include "serializer/serialize_options.hpp"
#include "serializer/serialize_string.hpp"
#include "serializer/serializer.hpp"

using zuu::models::Arena;
using zuu::models::Array;
using zuu::models::Document;
using zuu::models::Object;
using zuu::models::Value;
using zuu::parser::ParseValue;
using zuu::serializer::Serialize;
using zuu::serializer::SerializeCompact;
using zuu::serializer::SerializeOptions;
using zuu::serializer::SerializePretty;
using zuu::serializer::WriteEscapedString;
using zuu::serializer::WriteFloat;
using zuu::serializer::WriteInteger;

namespace {

// -----------------------------------------------------------------------
// WriteInteger
// -----------------------------------------------------------------------

void
    test_write_integer() {
    struct Case {
        int64_t input;
        const char* expected;
    };

    const Case cases[] = {
        {0, "0"},
        {1, "1"},
        {-1, "-1"},
        {42, "42"},
        {-42, "-42"},
        {123456789, "123456789"},
        {std::numeric_limits<int64_t>::max(), "9223372036854775807"},
        {std::numeric_limits<int64_t>::min(), "-9223372036854775808"},
    };

    for (const auto& c : cases) {
        std::string out;
        WriteInteger(out, c.input);
        assert(out == c.expected);
    }

    // Appends -- doesn't clobber -- existing buffer content.
    {
        std::string out = "prefix:";
        WriteInteger(out, 7);
        assert(out == "prefix:7");
    }

    std::cout << "test_write_integer passed\n";
}

// -----------------------------------------------------------------------
// WriteFloat
// -----------------------------------------------------------------------

void
    test_write_float() {
    // Values whose to_chars rendering already contains '.', 'e', or 'E'
    // must be passed through untouched (no spurious ".0").
    {
        std::string out;
        WriteFloat(out, 3.14);
        assert(out == "3.14");
    }
    {
        std::string out;
        WriteFloat(out, 0.1);
        assert(out == "0.1");
    }
    {
        std::string out;
        WriteFloat(out, 1.5e10);
        assert(out == "1.5e+10");
    }
    {
        std::string out;
        WriteFloat(out, -3.25);
        assert(out == "-3.25");
    }

    // Values that render as a bare integer from to_chars (e.g. 3.0 -> "3")
    // must get a forced ".0" so they still read back as JsonType::Float.
    {
        std::string out;
        WriteFloat(out, 3.0);
        assert(out == "3.0");
    }
    {
        std::string out;
        WriteFloat(out, 0.0);
        assert(out == "0.0");
    }
    {
        std::string out;
        WriteFloat(out, -0.0);
        assert(out == "-0.0");
    }
    {
        std::string out;
        WriteFloat(out, 100.0);
        assert(out == "100.0");
    }

    // Non-finite values have no JSON representation and must fall back to
    // `null` rather than emitting text that wouldn't parse back as JSON.
    {
        std::string out;
        WriteFloat(out, std::numeric_limits<double>::quiet_NaN());
        assert(out == "null");
    }
    {
        std::string out;
        WriteFloat(out, std::numeric_limits<double>::infinity());
        assert(out == "null");
    }
    {
        std::string out;
        WriteFloat(out, -std::numeric_limits<double>::infinity());
        assert(out == "null");
    }

    // Extreme-magnitude doubles must fit comfortably in the fixed-size
    // stack buffer without truncation.
    {
        std::string out;
        WriteFloat(out, 2.2250738585072014e-308); // DBL_MIN (normal)
        assert(out.find("2.2250738585072014e-308") != std::string::npos);
    }
    {
        std::string out;
        WriteFloat(out, std::numeric_limits<double>::max());
        assert(!out.empty());
        // Round-trips through std::from_chars back to the same value.
        double parsed = 0.0;
        std::from_chars(out.data(), out.data() + out.size(), parsed);
        assert(parsed == std::numeric_limits<double>::max());
    }

    // Appends -- doesn't clobber -- existing buffer content.
    {
        std::string out = "prefix:";
        WriteFloat(out, 1.5);
        assert(out == "prefix:1.5");
    }

    std::cout << "test_write_float passed\n";
}

// -----------------------------------------------------------------------
// WriteEscapedString
// -----------------------------------------------------------------------

void
    test_write_escaped_string() {
    struct Case {
        const char* input;
        const char* expected;
    };

    const Case cases[] = {
        {"", "\"\""},
        {"hello", "\"hello\""},
        {"line\nbreak", "\"line\\nbreak\""},
        {"tab\there", "\"tab\\there\""},
        {"back\bspace", "\"back\\bspace\""},
        {"form\ffeed", "\"form\\ffeed\""},
        {"carriage\rreturn", "\"carriage\\rreturn\""},
        {"quote\"inside", "\"quote\\\"inside\""},
        {"back\\slash", "\"back\\\\slash\""},
        // '/' is not escaped -- it's permitted but not required by JSON.
        {"slash/ok", "\"slash/ok\""},
    };
    for (const auto& c : cases) {
        std::string out;
        WriteEscapedString(out, c.input);
        assert(out == c.expected);
    }

    // Every other control character below 0x20 falls back to \u00XX.
    {
        std::string out;
        WriteEscapedString(out, std::string_view{"\x01\x02\x1F", 3});
        assert(out == "\"\\u0001\\u0002\\u001f\"");
    }

    // Raw multi-byte UTF-8 is copied through unchanged.
    {
        std::string out;
        WriteEscapedString(out, "caf\xC3\xA9"); // "café"
        assert(out == "\"caf\xC3\xA9\"");
    }

    // A NUL byte embedded in the string content must still be escaped
    // (string_view carries an explicit length, not a C-string).
    {
        std::string out;
        WriteEscapedString(out, std::string_view{"a\0b", 3});
        const std::string_view expected{"\"a\\u0000b\"", 10};
        assert(out == expected);
    }

    // Appends -- doesn't clobber -- existing buffer content.
    {
        std::string out = "prefix:";
        WriteEscapedString(out, "x");
        assert(out == "prefix:\"x\"");
    }

    std::cout << "test_write_escaped_string passed\n";
}

// -----------------------------------------------------------------------
// Serialize -- scalars
// -----------------------------------------------------------------------

void
    test_serialize_scalars() {
    assert(SerializeCompact(Value{}) == "null");
    assert(SerializeCompact(Value{nullptr}) == "null");
    assert(SerializeCompact(Value{true}) == "true");
    assert(SerializeCompact(Value{false}) == "false");
    assert(SerializeCompact(Value{static_cast<int64_t>(42)}) == "42");
    assert(SerializeCompact(Value{static_cast<int64_t>(-7)}) == "-7");
    assert(SerializeCompact(Value{3.5}) == "3.5");
    assert(SerializeCompact(Value{std::string_view{"hi"}}) == "\"hi\"");
    assert(SerializeCompact(Value{std::string_view{}}) == "\"\"");

    std::cout << "test_serialize_scalars passed\n";
}

// -----------------------------------------------------------------------
// Serialize -- compact arrays/objects
// -----------------------------------------------------------------------

void
    test_serialize_compact_containers() {
    Document doc;

    // Empty containers.
    {
        Array::Builder builder;
        Array* array = builder.Build(doc.arena());
        assert(SerializeCompact(Value{array}) == "[]");
    }
    {
        Object::Builder builder;
        Object* object = builder.Build(doc.arena());
        assert(SerializeCompact(Value{object}) == "{}");
    }

    // Flat array of mixed scalar types.
    {
        Array::Builder builder;
        builder.Push(Value{static_cast<int64_t>(1)});
        builder.Push(Value{2.5});
        builder.Push(Value{doc.MakeString("three")});
        builder.Push(Value{true});
        builder.Push(Value{false});
        builder.Push(Value{nullptr});
        Array* array = builder.Build(doc.arena());
        assert(SerializeCompact(Value{array}) == R"([1,2.5,"three",true,false,null])");
    }

    // Flat object; members must render in insertion order.
    {
        Object::Builder builder;
        builder.Insert("name", Value{doc.MakeString("zuu")});
        builder.Insert("age", Value{static_cast<int64_t>(20)});
        builder.Insert("active", Value{true});
        Object* object = builder.Build(doc.arena());
        assert(SerializeCompact(Value{object}) == R"({"name":"zuu","age":20,"active":true})");
    }

    // Nested arrays and objects together, round-tripped through the
    // parser to build the tree (exercises real, parser-produced Values
    // rather than only hand-built ones).
    {
        constexpr auto json = R"({"values":[1,2,3],"nested":{"ok":true,"inner":{"deep":1}}})";
        Arena arena;
        size_t pos = 0;
        const auto result = ParseValue(json, pos, std::strlen(json), arena, 0);
        assert(result.has_value());
        assert(SerializeCompact(*result) == json);
    }

    // Object keys containing characters that need escaping.
    {
        Object::Builder builder;
        builder.Insert(doc.MakeString("line\nbreak"), Value{static_cast<int64_t>(1)});
        Object* object = builder.Build(doc.arena());
        assert(SerializeCompact(Value{object}) == R"({"line\nbreak":1})");
    }

    // Duplicate keys are preserved, not deduplicated.
    {
        Object::Builder builder;
        builder.Insert("a", Value{static_cast<int64_t>(1)});
        builder.Insert("a", Value{static_cast<int64_t>(2)});
        Object* object = builder.Build(doc.arena());
        assert(SerializeCompact(Value{object}) == R"({"a":1,"a":2})");
    }

    std::cout << "test_serialize_compact_containers passed\n";
}

// -----------------------------------------------------------------------
// Serialize -- pretty arrays/objects
// -----------------------------------------------------------------------

void
    test_serialize_pretty_containers() {
    // Empty containers stay on one line even in pretty mode -- there's
    // nothing to break onto its own line.
    {
        Document doc;
        Array::Builder builder;
        Array* array = builder.Build(doc.arena());
        assert(SerializePretty(Value{array}) == "[]");
    }
    {
        Document doc;
        Object::Builder builder;
        Object* object = builder.Build(doc.arena());
        assert(SerializePretty(Value{object}) == "{}");
    }

    // Flat array, default 2-space indent.
    {
        Document doc;
        Array::Builder builder;
        builder.Push(Value{static_cast<int64_t>(1)});
        builder.Push(Value{static_cast<int64_t>(2)});
        builder.Push(Value{static_cast<int64_t>(3)});
        Array* array = builder.Build(doc.arena());
        const std::string expected = "[\n  1,\n  2,\n  3\n]";
        assert(SerializePretty(Value{array}) == expected);
    }

    // Flat object, default 2-space indent, space after ':'.
    {
        Document doc;
        Object::Builder builder;
        builder.Insert("a", Value{static_cast<int64_t>(1)});
        builder.Insert("b", Value{true});
        Object* object = builder.Build(doc.arena());
        const std::string expected = "{\n  \"a\": 1,\n  \"b\": true\n}";
        assert(SerializePretty(Value{object}) == expected);
    }

    // Nested structure indents one extra level per nesting depth.
    {
        constexpr auto json = R"({"a":[1,2],"b":{"c":3}})";
        Arena arena;
        size_t pos = 0;
        const auto result = ParseValue(json, pos, std::strlen(json), arena, 0);
        assert(result.has_value());
        const std::string expected = "{\n"
                                     "  \"a\": [\n"
                                     "    1,\n"
                                     "    2\n"
                                     "  ],\n"
                                     "  \"b\": {\n"
                                     "    \"c\": 3\n"
                                     "  }\n"
                                     "}";
        assert(SerializePretty(*result) == expected);
    }

    // An empty container nested inside a non-empty one still collapses
    // to a single line at its own position.
    {
        constexpr auto json = R"({"a":[]})";
        Arena arena;
        size_t pos = 0;
        const auto result = ParseValue(json, pos, std::strlen(json), arena, 0);
        assert(result.has_value());
        assert(SerializePretty(*result) == "{\n  \"a\": []\n}");
    }

    // Custom indent size.
    {
        Document doc;
        Object::Builder builder;
        builder.Insert("a", Value{static_cast<int64_t>(1)});
        Object* object = builder.Build(doc.arena());
        assert(SerializePretty(Value{object}, 4) == "{\n    \"a\": 1\n}");
    }

    // indent_size of 0: newlines still appear, just with no leading
    // spaces on each line.
    {
        Document doc;
        Object::Builder builder;
        builder.Insert("a", Value{static_cast<int64_t>(1)});
        Object* object = builder.Build(doc.arena());
        assert(SerializePretty(Value{object}, 0) == "{\n\"a\": 1\n}");
    }

    std::cout << "test_serialize_pretty_containers passed\n";
}

// -----------------------------------------------------------------------
// Serialize -- options / convenience wrappers
// -----------------------------------------------------------------------

void
    test_serialize_options_and_wrappers() {
    Document doc;
    Object::Builder builder;
    builder.Insert("a", Value{static_cast<int64_t>(1)});
    Object* object = builder.Build(doc.arena());
    const Value value{object};

    // Default-constructed options are compact.
    assert(Serialize(value) == R"({"a":1})");
    assert(Serialize(value, SerializeOptions{}) == R"({"a":1})");

    // SerializeCompact matches explicit compact options.
    assert(SerializeCompact(value) == Serialize(value, SerializeOptions{.pretty = false}));

    // SerializePretty matches explicit pretty options with the same
    // indent size, for both the default and a custom indent.
    assert(SerializePretty(value) ==
           Serialize(value, SerializeOptions{.pretty = true, .indent_size = 2}));
    assert(SerializePretty(value, 4) ==
           Serialize(value, SerializeOptions{.pretty = true, .indent_size = 4}));

    std::cout << "test_serialize_options_and_wrappers passed\n";
}

// -----------------------------------------------------------------------
// Round-trip: parse -> serialize -> parse must reach a fixed point, and
// the parser must accept every compact and pretty rendering this
// serializer produces.
// -----------------------------------------------------------------------

void
    test_serialize_parse_round_trip() {
    const char* samples[] = {
        "0",
        "-1",
        "3.14",
        "1.5e10",
        "-2.2250738585072014e-308",
        "\"\"",
        "\"hello world\"",
        "\"line\\nbreak\\ttab\"",
        "[]",
        "{}",
        "[1,2,3]",
        "[1,[2,[3,[4,[5]]]]]",
        R"({"nested":{"a":{"b":{"c":1}}}})",
        R"({"values":[1,2,3],"nested":{"ok":true,"inner":{"deep":1}}})",
        "true",
        "false",
        "null",
    };

    for (const char* sample : samples) {
        const std::string_view json{sample};

        Arena arena1;
        size_t pos1 = 0;
        const auto parsed1 = ParseValue(json, pos1, json.size(), arena1, 0);
        assert(parsed1.has_value());

        // Compact form re-parses to the same compact form (a fixed point).
        const std::string compact = SerializeCompact(*parsed1);
        Arena arena2;
        size_t pos2 = 0;
        const auto parsed2 = ParseValue(compact, pos2, compact.size(), arena2, 0);
        assert(parsed2.has_value());
        assert(SerializeCompact(*parsed2) == compact);

        // Pretty form re-parses to the exact same compact form too --
        // pretty-printing must not change the represented value.
        const std::string pretty = SerializePretty(*parsed1);
        Arena arena3;
        size_t pos3 = 0;
        const auto parsed3 = ParseValue(pretty, pos3, pretty.size(), arena3, 0);
        assert(parsed3.has_value());
        assert(SerializeCompact(*parsed3) == compact);
    }

    std::cout << "test_serialize_parse_round_trip passed\n";
}

} // namespace

int
    main() {
    test_write_integer();
    test_write_float();
    test_write_escaped_string();
    test_serialize_scalars();
    test_serialize_compact_containers();
    test_serialize_pretty_containers();
    test_serialize_options_and_wrappers();
    test_serialize_parse_round_trip();

    std::cout << "All serializer tests passed successfully!\n";
    return 0;
}