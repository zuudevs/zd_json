/**
 * @file object_find_miss_large.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Object_Find_Miss_Large benchmark implementation
 * @version 0.7.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include "json_models_benchmarks.hpp"

using namespace constants;

void
    Object_Find_Miss_Large(benchmark::State& state) {
    Document document;
    Object* object = BuildFlatObject(document, kLargeObjectFieldCount);
    for (auto stateLoop : state) {
        auto result = object->Find("does_not_exist");
        benchmark::DoNotOptimize(result);
    }
}
