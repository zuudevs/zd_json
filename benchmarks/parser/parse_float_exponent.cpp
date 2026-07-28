/**
 * @file parse_float_exponent.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief ParseFloat_Exponent benchmark implementation
 * @version 0.7.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include "json_parser_benchmarks.hpp"
#include "parser/parse_float.hpp"

using namespace constants;

void
    ParseFloat_Exponent(benchmark::State& state) {
    const char* start = kExponentFloat.data();
    const char* end = start + kExponentFloat.size();

    for (auto stateLoop : state) {
        benchmark::DoNotOptimize(start);
        benchmark::DoNotOptimize(end);

        auto result = zuu::json::parser::ParseFloat(start, end);
        benchmark::DoNotOptimize(result);
    }
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
}
