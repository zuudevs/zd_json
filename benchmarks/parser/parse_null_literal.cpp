/**
 * @file parse_null_literal.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief ParseNull_Literal benchmark implementation
 * @version 0.7.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include "json_parser_benchmarks.hpp"
#include "parser/parse_null.hpp"

using namespace constants;

void
    ParseNull_Literal(benchmark::State& state) {
    const char* start = kNullLiteral.data();
    const char* end = start + kNullLiteral.size();

    for (auto stateLoop : state) {
        benchmark::DoNotOptimize(start);
        benchmark::DoNotOptimize(end);

        auto result = zuu::json::parser::ParseNull(start, end);
        benchmark::DoNotOptimize(result);
    }
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
}
