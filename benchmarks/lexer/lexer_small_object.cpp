/**
 * @file lexer_small_object.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Lexer_SmallObject benchmark implementation
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
    Lexer_SmallObject(benchmark::State& state) {
    for (auto stateLoop : state) {
        auto tokens = zuu::json::lexer::Tokenize(kSmallJson);
        auto values = zuu::json::lexer::LexValues(kSmallJson, tokens);
        benchmark::DoNotOptimize(tokens);
        benchmark::DoNotOptimize(values);
        benchmark::ClobberMemory();
    }

    const auto jsonSize = static_cast<int64_t>(kSmallJson.size());
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) * jsonSize);
    state.SetBytesProcessed(static_cast<int64_t>(state.iterations()) * jsonSize);
}
