/**
 * @file write_escaped_string_plain_long.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief WriteEscapedString_PlainLong benchmark implementation
 * @version 0.7.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include "json_serializer_benchmarks.hpp"

using namespace constants;

void
    WriteEscapedString_PlainLong(benchmark::State& state) {
    for (auto _ : state) {
        std::string out;
        WriteEscapedString(out, kPlainLongString);
        benchmark::DoNotOptimize(out);
    }
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
}
