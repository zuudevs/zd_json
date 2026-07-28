/**
 * @file parse_string_unicode_escapes.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief ParseString_UnicodeEscapes benchmark implementation
 * @version 0.7.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include "json_parser_benchmarks.hpp"
#include "parser/parse_string.hpp"

using namespace constants;

void
    ParseString_UnicodeEscapes(benchmark::State& state) {
    const char* start = kUnicodeEscapedString.data();
    const char* end = start + kUnicodeEscapedString.size();

    for (auto stateLoop : state) {
        benchmark::DoNotOptimize(start);
        benchmark::DoNotOptimize(end);

        auto result = zuu::json::parser::ParseString(start, end);
        benchmark::DoNotOptimize(result);
    }
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
}
