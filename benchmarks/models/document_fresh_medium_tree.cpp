/**
 * @file document_fresh_medium_tree.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Document_Fresh_MediumTree benchmark implementation
 * @version 0.7.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include "json_models_benchmarks.hpp"

using namespace constants;

void
    Document_Fresh_MediumTree(benchmark::State& state) {
    for (auto stateLoop : state) {
        Document document;
        document.set_root(BuildMediumTree(document));
        benchmark::DoNotOptimize(document.root());
        benchmark::ClobberMemory();
    }
}
