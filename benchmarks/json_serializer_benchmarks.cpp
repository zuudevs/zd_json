/**
 * @file json_serializer_benchmarks.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Benchmark suite for WriteInteger, WriteFloat, WriteEscapedString,
 *        and Serialize/SerializeCompact/SerializePretty.
 * @version 0.7.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include "serializer/json_serializer_benchmarks.hpp"
#include <benchmark/benchmark.h>

BENCHMARK(WriteInteger_Short);
BENCHMARK(WriteInteger_Long);
BENCHMARK(WriteInteger_Negative);
BENCHMARK(WriteInteger_Min);

BENCHMARK(WriteFloat_Simple);
BENCHMARK(WriteFloat_Exponent);
BENCHMARK(WriteFloat_WholeNumber);
BENCHMARK(WriteFloat_NonFinite);

BENCHMARK(WriteEscapedString_PlainShort);
BENCHMARK(WriteEscapedString_PlainLong);
BENCHMARK(WriteEscapedString_CommonEscapes);
BENCHMARK(WriteEscapedString_ControlEscapes);
BENCHMARK(WriteEscapedString_MultibyteUtf8Passthrough);

BENCHMARK(Serialize_FlatIntArray_Small_Compact);
BENCHMARK(Serialize_FlatIntArray_Large_Compact);
BENCHMARK(Serialize_FlatIntArray_Large_Pretty);

BENCHMARK(Serialize_MediumTree_Compact);
BENCHMARK(Serialize_MediumTree_Pretty2);
BENCHMARK(Serialize_MediumTree_Pretty4);

BENCHMARK(Serialize_LargeArrayOfObjects_Compact);
BENCHMARK(Serialize_LargeArrayOfObjects_Pretty);

BENCHMARK_MAIN();