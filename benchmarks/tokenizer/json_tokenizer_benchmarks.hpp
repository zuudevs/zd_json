/**
 * @file json_tokenizer_benchmarks.hpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Brief description
 * @version 0.7.0
 * @date 2026-07-28
 * 
 * @copyright Copyright (c) 2026
 */

#pragma once

#include <benchmark/benchmark.h>
#include <string>
#include <string_view>

void
    Tokenizer_SmallObject(benchmark::State& state);
void
    Tokenizer_MediumPayload(benchmark::State& state);
void
    Tokenizer_LargeArray(benchmark::State& state);
void
    Tokenizer_DeeplyNested(benchmark::State& state);
void
    Tokenizer_NumericHeavy(benchmark::State& state);

namespace constants {

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

[[nodiscard]] inline std::string
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

[[nodiscard]] inline std::string
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

[[nodiscard]] inline std::string
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

} // namespace constants