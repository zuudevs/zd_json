/**
 * @file document_traverse_medium_tree.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Document_Traverse_MediumTree benchmark implementation
 * @version 0.7.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include "json_models_benchmarks.hpp"

using namespace constants;

void
    Document_Traverse_MediumTree(benchmark::State& state) {
    Document document;
    document.set_root(BuildMediumTree(document));

    for (auto stateLoop : state) {
        int64_t sum = SumNumbers(document.root());
        benchmark::DoNotOptimize(sum);
    }
}
