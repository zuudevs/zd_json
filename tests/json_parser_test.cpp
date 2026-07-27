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

#include "parser/parse_float.hpp"
#include "parser/parse_integral.hpp"

using zuu::JsonErrc;
using zuu::parser::ParseFloat;
using zuu::parser::ParseIntegral;

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

} // namespace

int
    main() {
    test_parse_integral();
    test_parse_float();

    std::cout << "All parser tests passed successfully!\n";
    return 0;
}