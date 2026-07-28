/**
 * @file lexer_medium_values_only.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Lexer_MediumValuesOnly benchmark implementation
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
    Lexer_MediumValuesOnly(benchmark::State& state) {
    const auto tokens = zuu::json::lexer::Tokenize(kMediumJson);
    for (auto stateLoop : state) {
        auto values = zuu::json::lexer::LexValues(kMediumJson, tokens);
        benchmark::DoNotOptimize(values);
        benchmark::ClobberMemory();
    }

    const auto jsonSize = static_cast<int64_t>(kMediumJson.size());
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) * jsonSize);
    state.SetBytesProcessed(static_cast<int64_t>(state.iterations()) * jsonSize);
}
