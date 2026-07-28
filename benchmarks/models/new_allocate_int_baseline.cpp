/**
 * @file new_allocate_int_baseline.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief New_AllocateInt_Baseline benchmark implementation
 * @version 0.7.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include "json_models_benchmarks.hpp"

using namespace constants;

void
    New_AllocateInt_Baseline(benchmark::State& state) {
    for (auto stateLoop : state) {
        int* value = new int(42);
        benchmark::DoNotOptimize(value);
        benchmark::ClobberMemory();
        delete value;
    }
}
