/**
 * @file parse_float_slow_path_fallback.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief ParseFloat_SlowPathFallback benchmark implementation
 * @version 0.7.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include "json_parser_benchmarks.hpp"
#include "parser/parse_float.hpp"

using namespace constants;

void
    ParseFloat_SlowPathFallback(benchmark::State& state) {
    const char* start = kSlowPathFloat.data();
    const char* end = start + kSlowPathFloat.size();

    for (auto stateLoop : state) {
        benchmark::DoNotOptimize(start);
        benchmark::DoNotOptimize(end);

        auto result = zuu::json::parser::ParseFloat(start, end);
        benchmark::DoNotOptimize(result);
    }
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
}
