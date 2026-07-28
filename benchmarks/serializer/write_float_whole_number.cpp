/**
 * @file write_float_whole_number.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief WriteFloat_WholeNumber benchmark implementation
 * @version 0.7.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include "json_serializer_benchmarks.hpp"

using namespace constants;

void
    WriteFloat_WholeNumber(benchmark::State& state) {
    for (auto stateLoop : state) {
        std::string out;
        WriteFloat(out, kWholeFloat);
        benchmark::DoNotOptimize(out);
    }
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
}
