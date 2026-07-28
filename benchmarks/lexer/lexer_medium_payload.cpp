/**
 * @file lexer_medium_payload.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Lexer_MediumPayload benchmark implementation
 * @version 0.7.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include "json_lexer_benchmarks.hpp"
#include "lexer/lexer.hpp"
#include "lexer/tokenizer.hpp"

using namespace constants;

void
    Lexer_MediumPayload(benchmark::State& state) {
    for (auto stateLoop : state) {
        auto tokens = zuu::lexer::Tokenize(kMediumJson);
        auto values = zuu::lexer::LexValues(kMediumJson, tokens);
        benchmark::DoNotOptimize(tokens);
        benchmark::DoNotOptimize(values);
        benchmark::ClobberMemory();
    }

    const auto jsonSize = static_cast<int64_t>(kMediumJson.size());
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) * jsonSize);
    state.SetBytesProcessed(static_cast<int64_t>(state.iterations()) * jsonSize);
}
