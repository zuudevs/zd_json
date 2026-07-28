/**
 * @file arena_allocate_int_steady_state.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Arena_AllocateInt_SteadyState benchmark implementation
 * @version 0.7.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include "json_models_benchmarks.hpp"

using namespace constants;

void
    Arena_AllocateInt_SteadyState(benchmark::State& state) {
    Arena arena;
    for (int i = 0; i < 1024; ++i) {
        benchmark::DoNotOptimize(arena.Create<int>(i));
    }

    for (auto stateLoop : state) {
        int* value = arena.Create<int>(42);
        benchmark::DoNotOptimize(value);
        benchmark::ClobberMemory();
    }
}
