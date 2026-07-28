/**
 * @file document_traverse_large_array_of_objects.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Document_Traverse_LargeArrayOfObjects benchmark implementation
 * @version 0.7.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include "json_models_benchmarks.hpp"

using namespace constants;

void
    Document_Traverse_LargeArrayOfObjects(benchmark::State& state) {
    Document document;
    document.set_root(BuildLargeArrayOfObjects(document, kLargeArrayOfObjectsCount));

    for (auto stateLoop : state) {
        int64_t sum = SumNumbers(document.root());
        benchmark::DoNotOptimize(sum);
    }
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) * kLargeArrayOfObjectsCount);
}
