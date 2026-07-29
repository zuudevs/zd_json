/**
 * @file json_real_datasets_benchmarks.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Benchmark suite for zd_json on real-world datasets (Canada, CITM Catalog,
 *        GitHub Events, Jeopardy, Literal, Mesh, Twitter, Update Center).
 * @version 0.8.0
 * @date 2026-07-29
 *
 * @copyright Copyright (c) 2026
 */

#include "json_real_datasets_benchmarks.hpp"
#include <benchmark/benchmark.h>

BENCHMARK(Parse_Canada);
BENCHMARK(SerializeCompact_Canada);
BENCHMARK(SerializePretty_Canada);
BENCHMARK(Tokenize_Canada);
BENCHMARK(Traverse_Canada);

BENCHMARK(Parse_CitmCatalog);
BENCHMARK(SerializeCompact_CitmCatalog);
BENCHMARK(SerializePretty_CitmCatalog);
BENCHMARK(Tokenize_CitmCatalog);
BENCHMARK(Traverse_CitmCatalog);

BENCHMARK(Parse_GithubEvents);
BENCHMARK(SerializeCompact_GithubEvents);
BENCHMARK(SerializePretty_GithubEvents);
BENCHMARK(Tokenize_GithubEvents);
BENCHMARK(Traverse_GithubEvents);

BENCHMARK(Parse_Jeopardy);
BENCHMARK(SerializeCompact_Jeopardy);
BENCHMARK(SerializePretty_Jeopardy);
BENCHMARK(Tokenize_Jeopardy);
BENCHMARK(Traverse_Jeopardy);

BENCHMARK(Parse_Literal);
BENCHMARK(SerializeCompact_Literal);
BENCHMARK(SerializePretty_Literal);
BENCHMARK(Tokenize_Literal);
BENCHMARK(Traverse_Literal);

BENCHMARK(Parse_Mesh);
BENCHMARK(SerializeCompact_Mesh);
BENCHMARK(SerializePretty_Mesh);
BENCHMARK(Tokenize_Mesh);
BENCHMARK(Traverse_Mesh);

BENCHMARK(Parse_Twitter);
BENCHMARK(SerializeCompact_Twitter);
BENCHMARK(SerializePretty_Twitter);
BENCHMARK(Tokenize_Twitter);
BENCHMARK(Traverse_Twitter);

BENCHMARK(Parse_UpdateCenter);
BENCHMARK(SerializeCompact_UpdateCenter);
BENCHMARK(SerializePretty_UpdateCenter);
BENCHMARK(Tokenize_UpdateCenter);
BENCHMARK(Traverse_UpdateCenter);

BENCHMARK_MAIN();
