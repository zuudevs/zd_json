/**
 * @file json_lexer_benchmarks.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Benchmark suite for zd_json lexer (full pipeline & lexer only)
 * @version 0.7.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include "lexer/json_lexer_benchmarks.hpp"
#include <benchmark/benchmark.h>

BENCHMARK(Lexer_SmallObject);
BENCHMARK(Lexer_MediumPayload);
BENCHMARK(Lexer_LargeArray);
BENCHMARK(Lexer_DeeplyNested);
BENCHMARK(Lexer_NumericHeavy);
BENCHMARK(Lexer_MediumValuesOnly);

BENCHMARK_MAIN();
