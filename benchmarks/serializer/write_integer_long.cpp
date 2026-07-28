/**
 * @file write_integer_long.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief WriteInteger_Long benchmark implementation
 * @version 0.7.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include "json_serializer_benchmarks.hpp"

using namespace constants;

void
    WriteInteger_Long(benchmark::State& state) {
    for (auto stateLoop : state) {
        std::string out;
        WriteInteger(out, kLongInt);
        benchmark::DoNotOptimize(out);
    }
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
}
