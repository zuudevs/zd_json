/**
 * @file from_chars_float_baseline.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief FromChars_Float_Baseline benchmark implementation
 * @version 0.7.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include "json_parser_benchmarks.hpp"
#include <charconv>

using namespace constants;

void
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
