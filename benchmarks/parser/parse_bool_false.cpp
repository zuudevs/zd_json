/**
 * @file parse_bool_false.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief ParseBool_False benchmark implementation
 * @version 0.7.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include "json_parser_benchmarks.hpp"
#include "parser/parse_bool.hpp"

using namespace constants;

void
    ParseBool_False(benchmark::State& state) {
    const char* start = kFalseLiteral.data();
    const char* end = start + kFalseLiteral.size();

    for (auto stateLoop : state) {
        benchmark::DoNotOptimize(start);
        benchmark::DoNotOptimize(end);

        auto result = zuu::json::parser::ParseBool(start, end);
        benchmark::DoNotOptimize(result);
    }
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
}
