/**
 * @file json_tokenizer_benchmark.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Benchmark suite for zd_json lexer/tokenizer
 * @version 0.3.0
 * @date 2026-07-26
 *
 * @copyright Copyright (c) 2026
 */

#include "tokenizer/json_tokenizer_benchmarks.hpp"
#include <benchmark/benchmark.h>

BENCHMARK(Tokenizer_SmallObject);
BENCHMARK(Tokenizer_MediumPayload);
BENCHMARK(Tokenizer_LargeArray);
BENCHMARK(Tokenizer_DeeplyNested);
BENCHMARK(Tokenizer_NumericHeavy);

BENCHMARK_MAIN();