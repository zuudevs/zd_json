/**
 * @file object_find_hit_small.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Object_Find_Hit_Small benchmark implementation
 * @version 0.7.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include "json_models_benchmarks.hpp"

using namespace constants;

void
    Object_Find_Hit_Small(benchmark::State& state) {
    Document document;
    Object* object = BuildFlatObject(document, kSmallObjectFieldCount);
    for (auto stateLoop : state) {
        auto result = object->Find("field_3");
        benchmark::DoNotOptimize(result);
    }
}
