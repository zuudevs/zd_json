/**
 * @file write_float_non_finite.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief WriteFloat_NonFinite benchmark implementation
 * @version 0.7.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include "json_serializer_benchmarks.hpp"

using namespace constants;

void
    WriteFloat_NonFinite(benchmark::State& state) {
    for (auto _ : state) {
        std::string out;
        WriteFloat(out, kNaN);
        benchmark::DoNotOptimize(out);
    }
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
}
