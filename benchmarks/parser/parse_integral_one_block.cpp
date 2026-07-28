/**
 * @file parse_integral_one_block.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief ParseIntegral_OneBlock benchmark implementation
 * @version 0.7.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include "json_parser_benchmarks.hpp"
#include "parser/parse_integral.hpp"

using namespace constants;

void
    ParseIntegral_OneBlock(benchmark::State& state) {
    const char* start = kBlockInt.data();
    const char* end = start + kBlockInt.size();

    for (auto stateLoop : state) {
        benchmark::DoNotOptimize(start);
        benchmark::DoNotOptimize(end);

        auto result = zuu::json::parser::ParseIntegral(start, end);
        benchmark::DoNotOptimize(result);
    }
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
}
