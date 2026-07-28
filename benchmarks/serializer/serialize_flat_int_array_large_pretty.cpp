/**
 * @file serialize_flat_int_array_large_pretty.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Serialize_FlatIntArray_Large_Pretty benchmark implementation
 * @version 0.7.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include "json_serializer_benchmarks.hpp"

using namespace constants;

void
    Serialize_FlatIntArray_Large_Pretty(benchmark::State& state) {
    Document document;
    Value value{BuildFlatIntArray(document, kLargeArrayCount)};

    for (auto stateLoop : state) {
        std::string out = SerializePretty(value);
        benchmark::DoNotOptimize(out);
    }
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) * kLargeArrayCount);
}
