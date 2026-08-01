/**
 * @file json_real_datasets_benchmarks.hpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Declarations and dataset loader utilities for real-world dataset benchmarks.
 * @version 0.8.0
 * @date 2026-07-29
 *
 * @copyright Copyright (c) 2026
 */

#pragma once

#include <benchmark/benchmark.h>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include "lexer/tokenizer.hpp"
#include "zd_json/json.hpp"

void Parse_Canada(benchmark::State& state);
void SerializeCompact_Canada(benchmark::State& state);
void SerializePretty_Canada(benchmark::State& state);
void Tokenize_Canada(benchmark::State& state);
void Traverse_Canada(benchmark::State& state);

void Parse_CitmCatalog(benchmark::State& state);
void SerializeCompact_CitmCatalog(benchmark::State& state);
void SerializePretty_CitmCatalog(benchmark::State& state);
void Tokenize_CitmCatalog(benchmark::State& state);
void Traverse_CitmCatalog(benchmark::State& state);

void Parse_GithubEvents(benchmark::State& state);
void SerializeCompact_GithubEvents(benchmark::State& state);
void SerializePretty_GithubEvents(benchmark::State& state);
void Tokenize_GithubEvents(benchmark::State& state);
void Traverse_GithubEvents(benchmark::State& state);

void Parse_Jeopardy(benchmark::State& state);
void SerializeCompact_Jeopardy(benchmark::State& state);
void SerializePretty_Jeopardy(benchmark::State& state);
void Tokenize_Jeopardy(benchmark::State& state);
void Traverse_Jeopardy(benchmark::State& state);

void Parse_Literal(benchmark::State& state);
void SerializeCompact_Literal(benchmark::State& state);
void SerializePretty_Literal(benchmark::State& state);
void Tokenize_Literal(benchmark::State& state);
void Traverse_Literal(benchmark::State& state);

void Parse_Mesh(benchmark::State& state);
void SerializeCompact_Mesh(benchmark::State& state);
void SerializePretty_Mesh(benchmark::State& state);
void Tokenize_Mesh(benchmark::State& state);
void Traverse_Mesh(benchmark::State& state);

void Parse_Twitter(benchmark::State& state);
void SerializeCompact_Twitter(benchmark::State& state);
void SerializePretty_Twitter(benchmark::State& state);
void Tokenize_Twitter(benchmark::State& state);
void Traverse_Twitter(benchmark::State& state);

void Parse_UpdateCenter(benchmark::State& state);
void SerializeCompact_UpdateCenter(benchmark::State& state);
void SerializePretty_UpdateCenter(benchmark::State& state);
void Tokenize_UpdateCenter(benchmark::State& state);
void Traverse_UpdateCenter(benchmark::State& state);

namespace real_datasets {

[[nodiscard]] inline std::string LoadDatasetFile(const std::string& filename) {
    std::vector<std::string> search_paths;
#ifdef ZD_JSON_SAMPLES_DIR
    search_paths.push_back(std::string(ZD_JSON_SAMPLES_DIR) + "/" + filename);
#endif
    search_paths.push_back("assets/samples/" + filename);
    search_paths.push_back("../assets/samples/" + filename);
    search_paths.push_back("../../assets/samples/" + filename);
    search_paths.push_back("../../../assets/samples/" + filename);

    for (const auto& path : search_paths) {
        std::ifstream file(path, std::ios::binary);
        if (file.is_open()) {
            return std::string((std::istreambuf_iterator<char>(file)),
                               std::istreambuf_iterator<char>());
        }
    }
    throw std::runtime_error("Failed to locate dataset file: " + filename +
                             ". Make sure git submodules are checked out ('git submodule update --init --recursive').");
}

struct Datasets {
    static const std::string& GetCanada() {
        static const std::string data = LoadDatasetFile("canada.json");
        return data;
    }
    static const std::string& GetCitmCatalog() {
        static const std::string data = LoadDatasetFile("citm_catalog.json");
        return data;
    }
    static const std::string& GetGithubEvents() {
        static const std::string data = LoadDatasetFile("github_events.json");
        return data;
    }
    static const std::string& GetJeopardy() {
        static const std::string data = LoadDatasetFile("jeopardy.json");
        return data;
    }
    static const std::string& GetLiteral() {
        static const std::string data = LoadDatasetFile("literal.json");
        return data;
    }
    static const std::string& GetMesh() {
        static const std::string data = LoadDatasetFile("mesh.json");
        return data;
    }
    static const std::string& GetTwitter() {
        static const std::string data = LoadDatasetFile("twitter.json");
        return data;
    }
    static const std::string& GetUpdateCenter() {
        static const std::string data = LoadDatasetFile("update-center.json");
        return data;
    }
};

[[nodiscard]] inline int64_t CountNodes(const zuu::json::Value& value) {
    switch (value.type()) {
        case zuu::json::enums::JsonType::Array: {
            int64_t count = 1;
            const auto* arr = value.as_array().value();
            for (const auto& element : *arr) {
                count += CountNodes(element);
            }
            return count;
        }
        case zuu::json::enums::JsonType::Object: {
            int64_t count = 1;
            const auto* obj = value.as_object().value();
            for (const auto& member : *obj) {
                count += 1 + CountNodes(member.value);
            }
            return count;
        }
        default:
            return 1;
    }
}

} // namespace real_datasets
