/**
 * @file arena_cold_start_many_small_ints.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Arena_ColdStart_ManySmallInts benchmark implementation
 * @version 0.7.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include "json_models_benchmarks.hpp"

using namespace constants;

void
    Arena_ColdStart_ManySmallInts(benchmark::State& state) {
    for (auto stateLoop : state) {
        Arena arena;
        for (int i = 0; i < kMediumArrayCount; ++i) {
            benchmark::DoNotOptimize(arena.Create<int>(i));
        }
        benchmark::ClobberMemory();
    }
}
