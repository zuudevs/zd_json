/**
 * @file write_integer_negative.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief WriteInteger_Negative benchmark implementation
 * @version 0.7.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include "json_serializer_benchmarks.hpp"

using namespace constants;

void
    WriteInteger_Negative(benchmark::State& state) {
    for (auto _ : state) {
        std::string out;
        WriteInteger(out, kNegativeInt);
        benchmark::DoNotOptimize(out);
    }
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
}
