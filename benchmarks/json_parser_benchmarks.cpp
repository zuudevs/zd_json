/**
 * @file json_parser_benchmarks.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Benchmark suite for ParseIntegral, ParseFloat, ParseBool,
 *        ParseNull, ParseShortString, and ParseString, including a
 *        std::from_chars baseline for the numeric parsers.
 * @version 0.7.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include "parser/json_parser_benchmarks.hpp"
#include <benchmark/benchmark.h>

BENCHMARK(ParseIntegral_Short);
BENCHMARK(ParseIntegral_OneBlock);
BENCHMARK(ParseIntegral_Long);
BENCHMARK(ParseIntegral_Negative);
BENCHMARK(FromChars_Long_Baseline);

BENCHMARK(ParseFloat_Simple);
BENCHMARK(ParseFloat_Exponent);
BENCHMARK(ParseFloat_SlowPathFallback);
BENCHMARK(FromChars_Float_Baseline);

BENCHMARK(ParseBool_True);
BENCHMARK(ParseBool_False);
BENCHMARK(ParseNull_Literal);

BENCHMARK(ParseShortString_NoEscape);
BENCHMARK(ParseString_ShortNoEscape_Baseline);
BENCHMARK(ParseString_LongNoEscape);
BENCHMARK(ParseString_CommonEscapes);
BENCHMARK(ParseString_UnicodeEscapes);
BENCHMARK(ParseString_MultibyteUtf8Passthrough);

BENCHMARK_MAIN();