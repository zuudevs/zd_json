/**
 * @file document_large_array_of_objects.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Document_LargeArrayOfObjects benchmark implementation
 * @version 0.7.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include "json_models_benchmarks.hpp"

using namespace constants;

void
    Document_LargeArrayOfObjects(benchmark::State& state) {
    for (auto stateLoop : state) {
        Document document;
        document.set_root(BuildLargeArrayOfObjects(document, kLargeArrayOfObjectsCount));
        benchmark::DoNotOptimize(document.root());
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) * kLargeArrayOfObjectsCount);
}
