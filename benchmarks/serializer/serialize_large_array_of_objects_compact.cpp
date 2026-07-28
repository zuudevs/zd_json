/**
 * @file serialize_large_array_of_objects_compact.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Serialize_LargeArrayOfObjects_Compact benchmark implementation
 * @version 0.7.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include "json_serializer_benchmarks.hpp"

using namespace constants;

void
    Serialize_LargeArrayOfObjects_Compact(benchmark::State& state) {
    Document document;
    Value value = BuildLargeArrayOfObjects(document, kLargeArrayOfObjectsCount);

    for (auto _ : state) {
        std::string out = SerializeCompact(value);
        benchmark::DoNotOptimize(out);
    }
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) * kLargeArrayOfObjectsCount);
}
