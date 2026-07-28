/**
 * @file parse_bool_true.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief ParseBool_True benchmark implementation
 * @version 0.7.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include "json_parser_benchmarks.hpp"
#include "parser/parse_bool.hpp"

using namespace constants;

void
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
