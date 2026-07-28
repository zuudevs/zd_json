/**
 * @file from_chars_long_baseline.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief FromChars_Long_Baseline benchmark implementation
 * @version 0.7.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include "json_parser_benchmarks.hpp"
#include <charconv>
#include <cstdint>

using namespace constants;

void
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
