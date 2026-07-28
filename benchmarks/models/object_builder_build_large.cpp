/**
 * @file object_builder_build_large.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Object_Builder_Build_Large benchmark implementation
 * @version 0.7.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include "json_models_benchmarks.hpp"

using namespace constants;

void
    Object_Builder_Build_Large(benchmark::State& state) {
    for (auto stateLoop : state) {
        Document document;
        Object* object = BuildFlatObject(document, kLargeObjectFieldCount);
        benchmark::DoNotOptimize(object);
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) * kLargeObjectFieldCount);
}
