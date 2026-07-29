/**
 * @file update_center_benchmarks.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Benchmark implementations for update-center.json dataset.
 * @version 0.8.0
 * @date 2026-07-29
 *
 * @copyright Copyright (c) 2026
 */

#include "json_real_datasets_benchmarks.hpp"

using namespace real_datasets;

void
    Parse_UpdateCenter(benchmark::State& state) {
    const auto& json = Datasets::GetUpdateCenter();
    const auto jsonSize = static_cast<int64_t>(json.size());

    for (auto stateLoop : state) {
        auto docResult = zuu::json::Parse(json);
        benchmark::DoNotOptimize(docResult);
        benchmark::ClobberMemory();
    }

    state.SetBytesProcessed(static_cast<int64_t>(state.iterations()) * jsonSize);
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) * jsonSize);
}

void
    SerializeCompact_UpdateCenter(benchmark::State& state) {
    const auto& json = Datasets::GetUpdateCenter();
    auto docResult = zuu::json::Parse(json);
    if (!docResult.has_value()) {
        state.SkipWithError("Parse failed");
        return;
    }
    const auto& doc = docResult.value();

    for (auto stateLoop : state) {
        std::string out = zuu::json::SerializeCompact(doc.root());
        benchmark::DoNotOptimize(out);
        benchmark::ClobberMemory();
    }

    state.SetBytesProcessed(static_cast<int64_t>(state.iterations()) *
                            static_cast<int64_t>(json.size()));
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) *
                            static_cast<int64_t>(json.size()));
}

void
    SerializePretty_UpdateCenter(benchmark::State& state) {
    const auto& json = Datasets::GetUpdateCenter();
    auto docResult = zuu::json::Parse(json);
    if (!docResult.has_value()) {
        state.SkipWithError("Parse failed");
        return;
    }
    const auto& doc = docResult.value();

    for (auto stateLoop : state) {
        std::string out = zuu::json::SerializePretty(doc.root());
        benchmark::DoNotOptimize(out);
        benchmark::ClobberMemory();
    }

    state.SetBytesProcessed(static_cast<int64_t>(state.iterations()) *
                            static_cast<int64_t>(json.size()));
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) *
                            static_cast<int64_t>(json.size()));
}

void
    Tokenize_UpdateCenter(benchmark::State& state) {
    const auto& json = Datasets::GetUpdateCenter();
    const auto jsonSize = static_cast<int64_t>(json.size());

    for (auto stateLoop : state) {
        auto tokens = zuu::json::lexer::Tokenize(json);
        benchmark::DoNotOptimize(tokens);
        benchmark::ClobberMemory();
    }

    state.SetBytesProcessed(static_cast<int64_t>(state.iterations()) * jsonSize);
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) * jsonSize);
}

void
    Traverse_UpdateCenter(benchmark::State& state) {
    const auto& json = Datasets::GetUpdateCenter();
    auto docResult = zuu::json::Parse(json);
    if (!docResult.has_value()) {
        state.SkipWithError("Parse failed");
        return;
    }
    const auto& doc = docResult.value();

    for (auto stateLoop : state) {
        int64_t nodes = CountNodes(doc.root());
        benchmark::DoNotOptimize(nodes);
    }
}
