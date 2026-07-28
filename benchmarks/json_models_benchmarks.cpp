/**
 * @file json_models_benchmarks.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Benchmark suite for zd_json DOM (Arena, Value, Array, Object,
 *        Document).
 * @version 0.7.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include "models/json_models_benchmarks.hpp"
#include <benchmark/benchmark.h>

BENCHMARK(Arena_AllocateInt_SteadyState);
BENCHMARK(New_AllocateInt_Baseline);
BENCHMARK(Arena_ColdStart_ManySmallInts);

BENCHMARK(Array_Builder_Build_Small);
BENCHMARK(Array_Builder_Build_Medium);
BENCHMARK(Array_Builder_Build_Large);

BENCHMARK(Object_Builder_Build_Small);
BENCHMARK(Object_Builder_Build_Large);

BENCHMARK(Object_Find_Hit_Small);
BENCHMARK(Object_Find_Hit_Large);
BENCHMARK(Object_Find_Miss_Large);

BENCHMARK(Document_Fresh_MediumTree);
BENCHMARK(Document_Reused_MediumTree);
BENCHMARK(Document_LargeArrayOfObjects);
BENCHMARK(Document_Traverse_MediumTree);
BENCHMARK(Document_Traverse_LargeArrayOfObjects);

BENCHMARK_MAIN();