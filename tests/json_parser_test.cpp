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

#include "parser/parse_bool.hpp"
#include "parser/parse_float.hpp"
#include "parser/parse_integral.hpp"
#include "parser/parse_literal.hpp"
#include "parser/parse_null.hpp"
#include "parser/parse_string.hpp"

using zuu::JsonErrc;
using zuu::parser::ParseBool;
using zuu::parser::ParseFloat;
using zuu::parser::ParseIntegral;
using zuu::parser::ParseLiteral;
using zuu::parser::ParseNull;
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
        assert(r.error() == JsonErrc::InvalidBooleanLiteral);
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
        assert(!r.has_value() && *r == -1);
        assert(r.error() == JsonErrc::InvalidNullLiteral);
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

} // namespace

int
    main() {
    test_parse_integral();
    test_parse_float();
    test_parse_bool();
    test_parse_null();
    test_parse_string();

    std::cout << "All parser tests passed successfully!\n";
    return 0;
}