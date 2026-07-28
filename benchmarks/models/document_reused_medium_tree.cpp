/**
 * @file document_reused_medium_tree.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Document_Reused_MediumTree benchmark implementation
 * @version 0.7.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include "json_models_benchmarks.hpp"

using namespace constants;

void
    Document_Reused_MediumTree(benchmark::State& state) {
    Document document;
    for (auto stateLoop : state) {
        document.Reset();
        document.set_root(BuildMediumTree(document));
        benchmark::DoNotOptimize(document.root());
        benchmark::ClobberMemory();
    }
}
