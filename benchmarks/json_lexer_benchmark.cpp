/**
 * @file json_lexer_benchmark.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Benchmark suite for zd_json lexer (full pipeline & lexer only)
 * @version 0.3.0
 * @date 2026-07-26
 *
 * @copyright Copyright (c) 2026
 */

#include <benchmark/benchmark.h>

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

#include "lexer/lexer.hpp"
#include "lexer/tokenizer.hpp"

namespace {

constexpr std::string_view kSmallJson = R"({"id": 1, "active": true, "name": "zuu"})";

constexpr std::string_view kMediumJson = R"({
    "user": {
        "id": 10482,
        "username": "antigravity_dev",
        "email": "dev@example.com",
        "is_active": true,
        "roles": ["admin", "developer", "tester"],
        "metadata": {
            "created_at": 1609459200,
            "last_login": 1774500000,
            "rating": 4.95
        }
    },
    "status": "success",
    "code": 200
})";

constexpr size_t kLargeArrayCapacity = 100000;
constexpr int kLargeArrayCount = 500;
constexpr double kLargeArrayValueMultiplier = 1.5;

constexpr size_t kDeepNestedCapacity = 2000;
constexpr int kDeepNestedDepth = 50;

constexpr size_t kNumericCapacity = 20000;
constexpr int kNumericCount = 1000;
constexpr double kNumericValueMultiplier = 123.456;

std::string
    generate_large_array_json() {
    std::string result = "[";
    result.reserve(kLargeArrayCapacity);
    for (int i = 0; i < kLargeArrayCount; ++i) {
        if (i > 0) {
            result += ",";
        }
        result += R"({"index":)" + std::to_string(i) + R"(,"value":)" +
                  std::to_string(static_cast<double>(i) * kLargeArrayValueMultiplier) +
                  R"(,"tag":"item_)" + std::to_string(i) + R"("})";
    }
    result += "]";
    return result;
}

std::string
    generate_deeply_nested_json() {
    std::string result;
    result.reserve(kDeepNestedCapacity);
    for (int i = 0; i < kDeepNestedDepth; ++i) {
        result += R"({"layer":)";
    }
    result += "42";
    for (int i = 0; i < kDeepNestedDepth; ++i) {
        result += "}";
    }
    return result;
}

std::string
    generate_numeric_json() {
    std::string result = "[";
    result.reserve(kNumericCapacity);
    for (int i = 0; i < kNumericCount; ++i) {
        if (i > 0) {
            result += ",";
        }
        result += std::to_string(static_cast<double>(i) * kNumericValueMultiplier);
    }
    result += "]";
    return result;
}

} // namespace

static void
    Lexer_SmallObject(benchmark::State& state) {
    for (auto stateLoop : state) {
        auto tokens = zuu::lexer::Tokenize(kSmallJson);
        auto values = zuu::lexer::LexValues(kSmallJson, tokens);
        benchmark::DoNotOptimize(tokens);
        benchmark::DoNotOptimize(values);
        benchmark::ClobberMemory();
    }

    const auto jsonSize = static_cast<int64_t>(kSmallJson.size());
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) * jsonSize);
    state.SetBytesProcessed(static_cast<int64_t>(state.iterations()) * jsonSize);
}

static void
    Lexer_MediumPayload(benchmark::State& state) {
    for (auto stateLoop : state) {
        auto tokens = zuu::lexer::Tokenize(kMediumJson);
        auto values = zuu::lexer::LexValues(kMediumJson, tokens);
        benchmark::DoNotOptimize(tokens);
        benchmark::DoNotOptimize(values);
        benchmark::ClobberMemory();
    }

    const auto jsonSize = static_cast<int64_t>(kMediumJson.size());
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) * jsonSize);
    state.SetBytesProcessed(static_cast<int64_t>(state.iterations()) * jsonSize);
}

static void
    Lexer_LargeArray(benchmark::State& state) {
    const std::string json = generate_large_array_json();
    for (auto stateLoop : state) {
        auto tokens = zuu::lexer::Tokenize(json);
        auto values = zuu::lexer::LexValues(json, tokens);
        benchmark::DoNotOptimize(tokens);
        benchmark::DoNotOptimize(values);
        benchmark::ClobberMemory();
    }

    const auto jsonSize = static_cast<int64_t>(json.size());
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) * jsonSize);
    state.SetBytesProcessed(static_cast<int64_t>(state.iterations()) * jsonSize);
}

static void
    Lexer_DeeplyNested(benchmark::State& state) {
    const std::string json = generate_deeply_nested_json();
    for (auto stateLoop : state) {
        auto tokens = zuu::lexer::Tokenize(json);
        auto values = zuu::lexer::LexValues(json, tokens);
        benchmark::DoNotOptimize(tokens);
        benchmark::DoNotOptimize(values);
        benchmark::ClobberMemory();
    }

    const auto jsonSize = static_cast<int64_t>(json.size());
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) * jsonSize);
    state.SetBytesProcessed(static_cast<int64_t>(state.iterations()) * jsonSize);
}

static void
    Lexer_NumericHeavy(benchmark::State& state) {
    const std::string json = generate_numeric_json();
    for (auto stateLoop : state) {
        auto tokens = zuu::lexer::Tokenize(json);
        auto values = zuu::lexer::LexValues(json, tokens);
        benchmark::DoNotOptimize(tokens);
        benchmark::DoNotOptimize(values);
        benchmark::ClobberMemory();
    }

    const auto jsonSize = static_cast<int64_t>(json.size());
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) * jsonSize);
    state.SetBytesProcessed(static_cast<int64_t>(state.iterations()) * jsonSize);
}

static void
    Lexer_MediumValuesOnly(benchmark::State& state) {
    const auto tokens = zuu::lexer::Tokenize(kMediumJson);
    for (auto stateLoop : state) {
        auto values = zuu::lexer::LexValues(kMediumJson, tokens);
        benchmark::DoNotOptimize(values);
        benchmark::ClobberMemory();
    }

    const auto jsonSize = static_cast<int64_t>(kMediumJson.size());
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) * jsonSize);
    state.SetBytesProcessed(static_cast<int64_t>(state.iterations()) * jsonSize);
}

BENCHMARK(Lexer_SmallObject);
BENCHMARK(Lexer_MediumPayload);
BENCHMARK(Lexer_LargeArray);
BENCHMARK(Lexer_DeeplyNested);
BENCHMARK(Lexer_NumericHeavy);
BENCHMARK(Lexer_MediumValuesOnly);

BENCHMARK_MAIN();
