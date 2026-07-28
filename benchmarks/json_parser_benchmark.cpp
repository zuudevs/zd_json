/**
 * @file json_parser_benchmark.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Benchmark suite for ParseIntegral, ParseFloat, ParseBool,
 *        ParseNull, ParseShortString, and ParseString, including a
 *        std::from_chars baseline for the numeric parsers.
 * @version 0.3.1
 * @date 2026-07-27
 *
 * @copyright Copyright (c) 2026
 */

#include <benchmark/benchmark.h>

#include <charconv>
#include <cstdint>
#include <string>
#include <string_view>

#include "parser/parse_bool.hpp"
#include "parser/parse_float.hpp"
#include "parser/parse_integral.hpp"
#include "parser/parse_null.hpp"
#include "parser/parse_string.hpp"

namespace {

constexpr std::string_view kShortInt = "42";                 // fits in the scalar tail path only
constexpr std::string_view kBlockInt = "12345678";           // exactly one SWAR block
constexpr std::string_view kLongInt = "1234567890123456789"; // two blocks + tail
constexpr std::string_view kNegativeInt = "-987654321012345";

constexpr std::string_view kSimpleFloat = "3.14";
constexpr std::string_view kExponentFloat = "6.02214076e23";
constexpr std::string_view kSlowPathFloat = "2.2250738585072014e-308"; // forces from_chars fallback

constexpr std::string_view kTrueLiteral = "true";
constexpr std::string_view kFalseLiteral = "false";
constexpr std::string_view kNullLiteral = "null";

// Short, escape-free string: eligible for the zero-copy ParseShortString path.
constexpr std::string_view kShortString = R"("hello")";

// Escape-free but past the Short threshold: still has to go through the
// allocating ParseString decode loop, one byte-copy at a time.
constexpr std::string_view kLongPlainString =
    R"("this_is_a_much_longer_string_with_no_escapes_at_all_just_ascii_text")";

// A realistic mix of common escape sequences.
constexpr std::string_view kEscapedString = R"("line1\nline2\ttabbed\\backslash\"quoted")";

// \uXXXX escapes, including a surrogate pair for an astral code point.
constexpr std::string_view kUnicodeEscapedString = R"("caf\u00e9 \uD83D\uDE00 \u4e2d\u6587")";

// Raw (non-escaped) multi-byte UTF-8, already validated by the lexer.
constexpr std::string_view kMultibyteUtf8String = "\"caf\xC3\xA9 \xE4\xB8\xAD\xE6\x96\x87 \xF0\x9F\x98\x80\"";

} // namespace

static void
    ParseIntegral_Short(benchmark::State& state) {
    const char* start = kShortInt.data();
    const char* end = start + kShortInt.size();

    for (auto stateLoop : state) {
        benchmark::DoNotOptimize(start);
        benchmark::DoNotOptimize(end);

        auto result = zuu::parser::ParseIntegral(start, end);
        benchmark::DoNotOptimize(result);
    }
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
}

static void
    ParseIntegral_OneBlock(benchmark::State& state) {
    const char* start = kBlockInt.data();
    const char* end = start + kBlockInt.size();

    for (auto stateLoop : state) {
        benchmark::DoNotOptimize(start);
        benchmark::DoNotOptimize(end);

        auto result = zuu::parser::ParseIntegral(start, end);
        benchmark::DoNotOptimize(result);
    }
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
}

static void
    ParseIntegral_Long(benchmark::State& state) {
    const char* start = kLongInt.data();
    const char* end = start + kLongInt.size();

    for (auto stateLoop : state) {
        benchmark::DoNotOptimize(start);
        benchmark::DoNotOptimize(end);

        auto result = zuu::parser::ParseIntegral(start, end);
		benchmark::DoNotOptimize(result);
    }
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
}

static void
    ParseIntegral_Negative(benchmark::State& state) {
    const char* start = kNegativeInt.data();
    const char* end = start + kNegativeInt.size();

    for (auto stateLoop : state) {
        benchmark::DoNotOptimize(start);
        benchmark::DoNotOptimize(end);

        auto result = zuu::parser::ParseIntegral(start, end);

        benchmark::DoNotOptimize(result);
    }
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
}

// Baseline: std::from_chars on the same long-integer input, for comparison
// against the SWAR fast path above.
static void
    FromChars_Long_Baseline(benchmark::State& state) {
    const char* start = kLongInt.data();
    const char* end = start + kLongInt.size();

    for (auto stateLoop : state) {
        benchmark::DoNotOptimize(start);
        benchmark::DoNotOptimize(end);

        uint64_t value{};
        auto res = std::from_chars(start, end, value);

        benchmark::DoNotOptimize(res);
        benchmark::DoNotOptimize(value);
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
}

static void
    ParseFloat_Simple(benchmark::State& state) {
    const char* start = kSimpleFloat.data();
    const char* end = start + kSimpleFloat.size();

    for (auto stateLoop : state) {
        benchmark::DoNotOptimize(start);
        benchmark::DoNotOptimize(end);

        auto result = zuu::parser::ParseFloat(start, end);
        benchmark::DoNotOptimize(result);
    }
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
}

static void
    ParseFloat_Exponent(benchmark::State& state) {
    const char* start = kExponentFloat.data();
    const char* end = start + kExponentFloat.size();

    for (auto stateLoop : state) {
        benchmark::DoNotOptimize(start);
        benchmark::DoNotOptimize(end);

        auto result = zuu::parser::ParseFloat(start, end);
        benchmark::DoNotOptimize(result);
    }
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
}

// Exercises the std::from_chars fallback path (fast-path bounds exceeded).
static void
    ParseFloat_SlowPathFallback(benchmark::State& state) {
    const char* start = kSlowPathFloat.data();
    const char* end = start + kSlowPathFloat.size();

    for (auto stateLoop : state) {
        benchmark::DoNotOptimize(start);
        benchmark::DoNotOptimize(end);

        auto result = zuu::parser::ParseFloat(start, end);
        benchmark::DoNotOptimize(result);
    }
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
}

// Baseline: std::from_chars on the same simple-float input, for comparison
// against the fast path above.
static void
    FromChars_Float_Baseline(benchmark::State& state) {
    const char* start = kSimpleFloat.data();
    const char* end = start + kSimpleFloat.size();

    for (auto stateLoop : state) {
        benchmark::DoNotOptimize(start);
        benchmark::DoNotOptimize(end);

        double value{};
        auto res = std::from_chars(start, end, value);

        benchmark::DoNotOptimize(res);
        benchmark::DoNotOptimize(value);
		benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
}

static void
    ParseBool_True(benchmark::State& state) {
    const char* start = kTrueLiteral.data();
    const char* end = start + kTrueLiteral.size();

    for (auto stateLoop : state) {
        benchmark::DoNotOptimize(start);
        benchmark::DoNotOptimize(end);

        auto result = zuu::parser::ParseBool(start, end);
        benchmark::DoNotOptimize(result);
    }
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
}

static void
    ParseBool_False(benchmark::State& state) {
    const char* start = kFalseLiteral.data();
    const char* end = start + kFalseLiteral.size();

    for (auto stateLoop : state) {
        benchmark::DoNotOptimize(start);
        benchmark::DoNotOptimize(end);

        auto result = zuu::parser::ParseBool(start, end);
        benchmark::DoNotOptimize(result);
    }
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
}

static void
    ParseNull_Literal(benchmark::State& state) {
    const char* start = kNullLiteral.data();
    const char* end = start + kNullLiteral.size();

    for (auto stateLoop : state) {
        benchmark::DoNotOptimize(start);
        benchmark::DoNotOptimize(end);

        auto result = zuu::parser::ParseNull(start, end);
        benchmark::DoNotOptimize(result);
    }
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
}

// Zero-copy fast path: no allocation, just quote-stripping.
static void
    ParseShortString_NoEscape(benchmark::State& state) {
    const char* start = kShortString.data();
    const char* end = start + kShortString.size();

    for (auto stateLoop : state) {
        benchmark::DoNotOptimize(start);
        benchmark::DoNotOptimize(end);

        auto result = zuu::parser::ParseShortString(start, end);
        benchmark::DoNotOptimize(result);
    }
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
}

// Same content as ParseShortString_NoEscape but routed through the
// allocating ParseString decode loop, to quantify the fast-path win.
static void
    ParseString_ShortNoEscape_Baseline(benchmark::State& state) {
    const char* start = kShortString.data();
    const char* end = start + kShortString.size();

    for (auto stateLoop : state) {
        benchmark::DoNotOptimize(start);
        benchmark::DoNotOptimize(end);

        auto result = zuu::parser::ParseString(start, end);
        benchmark::DoNotOptimize(result);
    }
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
}

// Long escape-free string: no escape decoding work, but still allocates and
// byte-copies through ParseString (JsonType::String, past the Short cutoff).
static void
    ParseString_LongNoEscape(benchmark::State& state) {
    const char* start = kLongPlainString.data();
    const char* end = start + kLongPlainString.size();

    for (auto stateLoop : state) {
        benchmark::DoNotOptimize(start);
        benchmark::DoNotOptimize(end);

        auto result = zuu::parser::ParseString(start, end);
        benchmark::DoNotOptimize(result);
    }
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
}

// A realistic mix of \n, \t, \\, and \" escapes.
static void
    ParseString_CommonEscapes(benchmark::State& state) {
    const char* start = kEscapedString.data();
    const char* end = start + kEscapedString.size();

    for (auto stateLoop : state) {
        benchmark::DoNotOptimize(start);
        benchmark::DoNotOptimize(end);

        auto result = zuu::parser::ParseString(start, end);
        benchmark::DoNotOptimize(result);
    }
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
}

// \uXXXX escapes, including a UTF-16 surrogate pair that must be combined
// into a single code point before re-encoding as UTF-8.
static void
    ParseString_UnicodeEscapes(benchmark::State& state) {
    const char* start = kUnicodeEscapedString.data();
    const char* end = start + kUnicodeEscapedString.size();

    for (auto stateLoop : state) {
        benchmark::DoNotOptimize(start);
        benchmark::DoNotOptimize(end);

        auto result = zuu::parser::ParseString(start, end);
        benchmark::DoNotOptimize(result);
    }
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
}

// Raw (non-escaped) multi-byte UTF-8 content, copied through byte-for-byte.
static void
    ParseString_MultibyteUtf8Passthrough(benchmark::State& state) {
    const char* start = kMultibyteUtf8String.data();
    const char* end = start + kMultibyteUtf8String.size();

    for (auto stateLoop : state) {
        benchmark::DoNotOptimize(start);
        benchmark::DoNotOptimize(end);

        auto result = zuu::parser::ParseString(start, end);
        benchmark::DoNotOptimize(result);
    }
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
}

BENCHMARK(ParseIntegral_Short);
BENCHMARK(ParseIntegral_OneBlock);
BENCHMARK(ParseIntegral_Long);
BENCHMARK(ParseIntegral_Negative);
BENCHMARK(FromChars_Long_Baseline);

BENCHMARK(ParseFloat_Simple);
BENCHMARK(ParseFloat_Exponent);
BENCHMARK(ParseFloat_SlowPathFallback);
BENCHMARK(FromChars_Float_Baseline);

BENCHMARK(ParseBool_True);
BENCHMARK(ParseBool_False);
BENCHMARK(ParseNull_Literal);

BENCHMARK(ParseShortString_NoEscape);
BENCHMARK(ParseString_ShortNoEscape_Baseline);
BENCHMARK(ParseString_LongNoEscape);
BENCHMARK(ParseString_CommonEscapes);
BENCHMARK(ParseString_UnicodeEscapes);
BENCHMARK(ParseString_MultibyteUtf8Passthrough);

BENCHMARK_MAIN();