/**
 * @file json_parser_test.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Test suite for ParseIntegral and ParseFloat
 * @version 0.3.1
 * @date 2026-07-27
 *
 * @copyright Copyright (c) 2026
 */

#include <cassert>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>

#include "models/arena.hpp"
#include "models/array.hpp"
#include "models/object.hpp"
#include "models/value.hpp"
#include "parser/parse_array.hpp"
#include "parser/parse_bool.hpp"
#include "parser/parse_float.hpp"
#include "parser/parse_integral.hpp"
#include "parser/parse_literal.hpp"
#include "parser/parse_null.hpp"
#include "parser/parse_object.hpp"
#include "parser/parse_string.hpp"

using zuu::JsonErrc;
using zuu::models::Arena;
using zuu::models::Array;
using zuu::models::Object;
using zuu::models::Value;
using zuu::parser::ParseArray;
using zuu::parser::ParseBool;
using zuu::parser::ParseFloat;
using zuu::parser::ParseIntegral;
using zuu::parser::ParseLiteral;
using zuu::parser::ParseNull;
using zuu::parser::ParseObject;
using zuu::parser::ParseShortString;
using zuu::parser::ParseString;

namespace {

[[nodiscard]] bool
    approx_equal(double a, double b) {
    if (a == b) {
        return true;
    }
    const double diff = std::fabs(a - b);
    const double scale = std::max(std::fabs(a), std::fabs(b));
    return diff <= scale * 1e-12;
}

void
    test_parse_integral() {
    // Values spanning every SWAR block boundary: 1..17 digits, so both the
    // 8-byte fast path and the scalar tail path (and their handoff) are
    // exercised, including inputs with no tail digits at all (exactly 8/16).
    for (int n = 1; n <= 17; ++n) {
        const std::string digits(static_cast<size_t>(n), '7');
        const auto result = ParseIntegral(digits.data(), digits.data() + digits.size());
        assert(result.has_value());
        assert(*result == std::strtoull(digits.c_str(), nullptr, 10));
    }

    {
        constexpr auto s = "12345678"; // exactly one full 8-byte block, no tail
        const auto r = ParseIntegral(s, s + 8);
        assert(r.has_value() && *r == 12345678ULL);
    }
    {
        constexpr auto s = "1234567890123456"; // exactly two full blocks
        const auto r = ParseIntegral(s, s + 16);
        assert(r.has_value() && *r == 1234567890123456ULL);
    }
    {
        constexpr auto s = "-123456789012"; // negative, crosses a block boundary
        const auto r = ParseIntegral(s, s + 13);
        assert(r.has_value() && *r == static_cast<uint64_t>(-123456789012LL));
    }
    {
        constexpr auto s = "18446744073709551615"; // UINT64_MAX
        const auto r = ParseIntegral(s, s + 20);
        assert(r.has_value() && *r == 18446744073709551615ULL);
    }
    {
        constexpr auto s = "0";
        const auto r = ParseIntegral(s, s + 1);
        assert(r.has_value() && *r == 0);
    }

    // Malformed input must return an error, never a garbage value.
    const char* invalid[] = {"", "-", "12a34", "1.5", "--5", "12 34", "1a2345678"};
    for (auto* s : invalid) {
        const auto r = ParseIntegral(s, s + std::strlen(s));
        assert(!r.has_value());
        assert(r.error() == JsonErrc::InvalidFormat);
    }

    std::cout << "test_parse_integral passed\n";
}

void
    test_parse_float() {
    struct Case {
        const char* input;
        double expected;
    };

    const Case cases[] = {
        {"3.14", 3.14},
        {"0.1", 0.1},
        {"-3.14", -3.14},
        {"1e10", 1e10},
        {"1E10", 1e10},
        {"1e-10", 1e-10},
        {"1.5e3", 1500.0},
        {"-1.5e-3", -0.0015},
        {"0", 0.0},
        {"0.0", 0.0},
        {"123456789.123456", 123456789.123456},
        // Outside the fast-path bounds: exercises the std::from_chars fallback.
        {"2.2250738585072014e-308", 2.2250738585072014e-308},
    };
    for (const auto& c : cases) {
        const auto r = ParseFloat(c.input, c.input + std::strlen(c.input));
        assert(r.has_value());
        assert(approx_equal(*r, c.expected));
    }

    // Malformed input must return an error, never a garbage value or a crash.
    const char* invalid[] = {"", "-", ".", "-.", "1e", "1e+", "1e-", "1.2.3", "e5", "1.2x3", "1x"};
    for (auto* s : invalid) {
        const auto r = ParseFloat(s, s + std::strlen(s));
        assert(!r.has_value());
        assert(r.error() == JsonErrc::InvalidFormat);
    }

    std::cout << "test_parse_float passed\n";
}

void
    test_parse_bool() {
    {
        constexpr auto s = "true";
        const auto r = ParseLiteral(s, s + 4);
        assert(r.has_value());
        if (auto v = static_cast<bool>(*r); *r != -1) {
            assert(v == true);
        }
    }
    {
        constexpr auto s = "false";
        const auto r = ParseLiteral(s, s + 5);
        assert(r.has_value());
        if (auto v = static_cast<bool>(*r); *r != -1) {
            assert(v == false);
        }
    }

    const char* invalid[] = {"", "tru", "True", "falze", "truee", "fals", "nulll"};
    for (auto* s : invalid) {
        const auto r = ParseLiteral(s, s + std::strlen(s));
        assert(!r.has_value());
        assert(r.error() == JsonErrc::InvalidValue);
    }

    std::cout << "test_parse_bool passed\n";
}

void
    test_parse_null() {
    {
        constexpr auto s = "null";
        const auto r = ParseLiteral(s, s + 4);
        assert(r.has_value() && *r == -1);
    }

    const char* invalid[] = {"", "nul", "Null", "nulll", "NULL"};
    for (auto* s : invalid) {
        const auto r = ParseLiteral(s, s + std::strlen(s));
        assert(!r.has_value());
        assert(r.error() == JsonErrc::InvalidValue);
    }

    std::cout << "test_parse_null passed\n";
}

void
    test_parse_string() {
    // ParseShortString: zero-copy, unescaped content only.
    {
        constexpr auto s = R"("hello")";
        const auto r = ParseShortString(s, s + std::strlen(s));
        assert(r.has_value());
        assert(*r == "hello");
    }
    {
        constexpr auto s = R"("")";
        const auto r = ParseShortString(s, s + 2);
        assert(r.has_value());
        assert(r->empty());
    }
    {
        constexpr auto s = "hello"; // missing quotes
        const auto r = ParseShortString(s, s + 5);
        assert(!r.has_value());
        assert(r.error() == JsonErrc::InvalidFormat);
    }

    // ParseString: full escape decoding.
    struct Case {
        const char* input;
        const char* expected;
    };
    const Case cases[] = {
        {R"("hello")", "hello"},
        {R"("")", ""},
        {R"("line\nbreak")", "line\nbreak"},
        {R"("tab\there")", "tab\there"},
        {R"("quote\"inside")", "quote\"inside"},
        {R"("back\\slash")", "back\\slash"},
        {R"("slash\/ok")", "slash/ok"},
        {R"("\u0041\u0042\u0043")", "ABC"},
        // U+1F600 GRINNING FACE via a UTF-16 surrogate pair.
        {R"("emoji:\uD83D\uDE00")", "emoji:\xF0\x9F\x98\x80"},
    };
    for (const auto& c : cases) {
        const auto r = ParseString(c.input, c.input + std::strlen(c.input));
        assert(r.has_value());
        assert(*r == c.expected);
    }

    // Raw multi-byte UTF-8 is copied through untouched.
    {
        constexpr auto s = "\"caf\xC3\xA9\""; // "café"
        const auto r = ParseString(s, s + std::strlen(s));
        assert(r.has_value());
        assert(*r == "caf\xC3\xA9");
    }

    struct BadCase {
        const char* input;
        JsonErrc expected_err;
    };
    const BadCase bad_cases[] = {
        {"hello", JsonErrc::InvalidFormat},                // missing quotes
        {R"("bad\x")", JsonErrc::InvalidValue},            // unknown escape
        {R"("bad\u12")", JsonErrc::InvalidValue},          // incomplete \u escape
        {R"("bad\uZZZZ")", JsonErrc::InvalidValue},        // non-hex \u digits
        {R"("\uD83Dnotlow")", JsonErrc::InvalidSurrogate}, // high surrogate w/o low
        {R"("\uDE00")", JsonErrc::InvalidSurrogate},       // lone low surrogate
    };
    for (const auto& bc : bad_cases) {
        const auto r = ParseString(bc.input, bc.input + std::strlen(bc.input));
        assert(!r.has_value());
        assert(r.error() == bc.expected_err);
    }

    // Unescaped raw control character.
    {
        std::string s = "\"bad";
        s.push_back('\x01');
        s += "char\"";
        const auto r = ParseString(s.c_str(), s.c_str() + s.size());
        assert(!r.has_value());
        assert(r.error() == JsonErrc::UnescapedCharacter);
    }

    std::cout << "test_parse_string passed\n";
}

void
    test_parse_array() {
    Arena arena;

    // Empty array.
    {
        constexpr auto json = "[]";
        size_t pos = 0;
        const auto result = ParseArray(json, pos, std::strlen(json), arena, 0);
        assert(result.has_value());
        assert((*result)->empty());
        assert(pos == std::strlen(json));
    }

    // Flat array of mixed scalar types.
    {
        constexpr auto json = R"([1, 2.5, "three", true, false, null])";
        size_t pos = 0;
        const auto result = ParseArray(json, pos, std::strlen(json), arena, 0);
        assert(result.has_value());
        const Array* array = *result;
        assert(array->size() == 6);
        assert(array->at(0).value()->as_integer().value() == 1);
        assert(approx_equal(array->at(1).value()->as_double().value(), 2.5));
        assert(array->at(2).value()->as_string().value() == "three");
        assert(array->at(3).value()->as_bool().value() == true);
        assert(array->at(4).value()->as_bool().value() == false);
        assert(array->at(5).value()->is_null());
        assert(pos == std::strlen(json));
    }

    // Nested arrays.
    {
        constexpr auto json = "[[1, 2], [3, [4, 5]]]";
        size_t pos = 0;
        const auto result = ParseArray(json, pos, std::strlen(json), arena, 0);
        assert(result.has_value());
        const Array* outer = *result;
        assert(outer->size() == 2);

        const Array* first = outer->at(0).value()->as_array().value();
        assert(first->size() == 2);
        assert(first->at(0).value()->as_integer().value() == 1);
        assert(first->at(1).value()->as_integer().value() == 2);

        const Array* second = outer->at(1).value()->as_array().value();
        assert(second->size() == 2);
        assert(second->at(0).value()->as_integer().value() == 3);

        const Array* nested = second->at(1).value()->as_array().value();
        assert(nested->size() == 2);
        assert(nested->at(1).value()->as_integer().value() == 5);
    }

    // Whitespace tolerance around brackets, commas, and elements.
    {
        constexpr auto json = "  [ 1 ,\n\t2 , 3 ]  ";
        size_t pos = 2; // caller is expected to point at '[' already
        const auto result = ParseArray(json, pos, std::strlen(json), arena, 0);
        assert(result.has_value());
        assert((*result)->size() == 3);
    }

    // Malformed input.
    {
        const char* s = "1, 2]"; // doesn't start with '['
        size_t pos = 0;
        const auto result = ParseArray(s, pos, std::strlen(s), arena, 0);
        assert(!result.has_value());
        assert(result.error() == JsonErrc::InvalidFormat);
    }
    {
        const char* s = "[1, 2"; // unterminated
        size_t pos = 0;
        const auto result = ParseArray(s, pos, std::strlen(s), arena, 0);
        assert(!result.has_value());
        assert(result.error() == JsonErrc::InvalidFormat);
    }
    {
        const char* s = "[1, 2,]"; // trailing comma
        size_t pos = 0;
        const auto result = ParseArray(s, pos, std::strlen(s), arena, 0);
        assert(!result.has_value());
        assert(result.error() == JsonErrc::TrailingComma);
    }
    {
        const char* s = "[1 2]"; // missing comma
        size_t pos = 0;
        const auto result = ParseArray(s, pos, std::strlen(s), arena, 0);
        assert(!result.has_value());
        assert(result.error() == JsonErrc::MissingComma);
    }
    {
        const char* s = "[1, ]"; // missing element after comma
        size_t pos = 0;
        const auto result = ParseArray(s, pos, std::strlen(s), arena, 0);
        assert(!result.has_value());
        assert(result.error() == JsonErrc::TrailingComma);
    }

    std::cout << "test_parse_array passed\n";
}

void
    test_parse_object() {
    Arena arena;

    // Empty object.
    {
        constexpr auto json = "{}";
        size_t pos = 0;
        const auto result = ParseObject(json, pos, std::strlen(json), arena, 0);
        assert(result.has_value());
        assert((*result)->empty());
        assert(pos == std::strlen(json));
    }

    // Flat object with mixed member value types.
    {
        constexpr auto json =
            R"({"name": "zuu", "age": 20, "score": 98.5, "active": true, "extra": null})";
        size_t pos = 0;
        const auto result = ParseObject(json, pos, std::strlen(json), arena, 0);
        assert(result.has_value());
        const Object* object = *result;
        assert(object->size() == 5);
        assert(object->Find("name").value()->as_string().value() == "zuu");
        assert(object->Find("age").value()->as_integer().value() == 20);
        assert(approx_equal(object->Find("score").value()->as_double().value(), 98.5));
        assert(object->Find("active").value()->as_bool().value() == true);
        assert(object->Find("extra").value()->is_null());
        assert(pos == std::strlen(json));
    }

    // Nested objects and arrays together.
    {
        constexpr auto json =
            R"({"values": [1, 2, 3], "nested": {"ok": true, "inner": {"deep": 1}}})";
        size_t pos = 0;
        const auto result = ParseObject(json, pos, std::strlen(json), arena, 0);
        assert(result.has_value());
        const Object* object = *result;

        const Array* values = object->Find("values").value()->as_array().value();
        assert(values->size() == 3);
        assert(values->at(2).value()->as_integer().value() == 3);

        const Object* nested = object->Find("nested").value()->as_object().value();
        assert(nested->Find("ok").value()->as_bool().value() == true);

        const Object* inner = nested->Find("inner").value()->as_object().value();
        assert(inner->Find("deep").value()->as_integer().value() == 1);
    }

    // A key containing an escape sequence must be decoded, not left raw.
    {
        constexpr auto json = R"({"line\nbreak": 1})";
        size_t pos = 0;
        const auto result = ParseObject(json, pos, std::strlen(json), arena, 0);
        assert(result.has_value());
        assert((*result)->Find("line\nbreak").value()->as_integer().value() == 1);
    }

    // Duplicate keys are preserved in insertion order; Find() returns the
    // first match, matching models::Object's own documented semantics.
    {
        constexpr auto json = R"({"a": 1, "a": 2})";
        size_t pos = 0;
        const auto result = ParseObject(json, pos, std::strlen(json), arena, 0);
        assert(result.has_value());
        assert((*result)->size() == 2);
        assert((*result)->Find("a").value()->as_integer().value() == 1);
    }

    // Malformed input.
    {
        const char* s = R"("a": 1})"; // doesn't start with '{'
        size_t pos = 0;
        const auto result = ParseObject(s, pos, std::strlen(s), arena, 0);
        assert(!result.has_value());
        assert(result.error() == JsonErrc::InvalidFormat);
    }
    {
        const char* s = R"({"a": 1)"; // unterminated
        size_t pos = 0;
        const auto result = ParseObject(s, pos, std::strlen(s), arena, 0);
        assert(!result.has_value());
        assert(result.error() == JsonErrc::InvalidFormat);
    }
    {
        const char* s = R"({a: 1})"; // unquoted key
        size_t pos = 0;
        const auto result = ParseObject(s, pos, std::strlen(s), arena, 0);
        assert(!result.has_value());
        assert(result.error() == JsonErrc::UnquotedKey);
    }
    {
        const char* s = "{'a': 1}"; // single-quoted key
        size_t pos = 0;
        const auto result = ParseObject(s, pos, std::strlen(s), arena, 0);
        assert(!result.has_value());
        assert(result.error() == JsonErrc::SingleQuotedString);
    }
    {
        const char* s = R"({"a" 1})"; // missing colon
        size_t pos = 0;
        const auto result = ParseObject(s, pos, std::strlen(s), arena, 0);
        assert(!result.has_value());
        assert(result.error() == JsonErrc::InvalidFormat);
    }
    {
        const char* s = R"({"a": 1,})"; // trailing comma
        size_t pos = 0;
        const auto result = ParseObject(s, pos, std::strlen(s), arena, 0);
        assert(!result.has_value());
        assert(result.error() == JsonErrc::TrailingComma);
    }
    {
        const char* s = R"({"a": 1 "b": 2})"; // missing comma
        size_t pos = 0;
        const auto result = ParseObject(s, pos, std::strlen(s), arena, 0);
        assert(!result.has_value());
        assert(result.error() == JsonErrc::MissingComma);
    }

    std::cout << "test_parse_object passed\n";
}

} // namespace

int
    main() {
    test_parse_integral();
    test_parse_float();
    test_parse_bool();
    test_parse_null();
    test_parse_string();
    test_parse_array();
    test_parse_object();

    std::cout << "All parser tests passed successfully!\n";
    return 0;
}