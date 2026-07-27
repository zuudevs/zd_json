/**
 * @file json_parser_benchmark.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Benchmark suite for ParseIntegral and ParseFloat, including a
 *        std::from_chars baseline for comparison
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

#include "parser/parse_float.hpp"
#include "parser/parse_integral.hpp"

namespace {

constexpr std::string_view kShortInt = "42";                 // fits in the scalar tail path only
constexpr std::string_view kBlockInt = "12345678";           // exactly one SWAR block
constexpr std::string_view kLongInt = "1234567890123456789"; // two blocks + tail
constexpr std::string_view kNegativeInt = "-987654321012345";

constexpr std::string_view kSimpleFloat = "3.14";
constexpr std::string_view kExponentFloat = "6.02214076e23";
constexpr std::string_view kSlowPathFloat = "2.2250738585072014e-308"; // forces from_chars fallback

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

BENCHMARK(ParseIntegral_Short);
BENCHMARK(ParseIntegral_OneBlock);
BENCHMARK(ParseIntegral_Long);
BENCHMARK(ParseIntegral_Negative);
BENCHMARK(FromChars_Long_Baseline);

BENCHMARK(ParseFloat_Simple);
BENCHMARK(ParseFloat_Exponent);
BENCHMARK(ParseFloat_SlowPathFallback);
BENCHMARK(FromChars_Float_Baseline);

BENCHMARK_MAIN();