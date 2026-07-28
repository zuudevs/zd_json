/**
 * @file tokenizer_medium_payload.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Brief description
 * @version 0.6.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include "json_tokenizer_benchmarks.hpp"
#include "lexer/tokenizer.hpp"

using namespace constants;

void
    Tokenizer_MediumPayload(benchmark::State& state) {
    for (auto stateLoop : state) {
        auto tokens = zuu::json::lexer::Tokenize(kMediumJson);
        benchmark::DoNotOptimize(tokens);
        benchmark::ClobberMemory();
    }

    const auto jsonSize = static_cast<int64_t>(kMediumJson.size());
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) * jsonSize);
    state.SetBytesProcessed(static_cast<int64_t>(state.iterations()) * jsonSize);
}