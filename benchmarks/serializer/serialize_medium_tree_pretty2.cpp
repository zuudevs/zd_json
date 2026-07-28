/**
 * @file serialize_medium_tree_pretty2.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Serialize_MediumTree_Pretty2 benchmark implementation
 * @version 0.7.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include "json_serializer_benchmarks.hpp"

using namespace constants;

void
    Serialize_MediumTree_Pretty2(benchmark::State& state) {
    Document document;
    Value value = BuildMediumTree(document);

    for (auto stateLoop : state) {
        std::string out = SerializePretty(value, 2);
        benchmark::DoNotOptimize(out);
    }
    state.SetBytesProcessed(static_cast<int64_t>(state.iterations()) *
                            static_cast<int64_t>(SerializePretty(value, 2).size()));
}
