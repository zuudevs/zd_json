/**
 * @file json_parser_benchmarks.hpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Benchmark function declarations and shared payload constants for zd_json parser benchmarks.
 * @version 0.7.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#pragma once

#include <benchmark/benchmark.h>
#include <string_view>

void
    ParseIntegral_Short(benchmark::State& state);
void
    ParseIntegral_OneBlock(benchmark::State& state);
void
    ParseIntegral_Long(benchmark::State& state);
void
    ParseIntegral_Negative(benchmark::State& state);
void
    FromChars_Long_Baseline(benchmark::State& state);

void
    ParseFloat_Simple(benchmark::State& state);
void
    ParseFloat_Exponent(benchmark::State& state);
void
    ParseFloat_SlowPathFallback(benchmark::State& state);
void
    FromChars_Float_Baseline(benchmark::State& state);

void
    ParseBool_True(benchmark::State& state);
void
    ParseBool_False(benchmark::State& state);
void
    ParseNull_Literal(benchmark::State& state);

void
    ParseShortString_NoEscape(benchmark::State& state);
void
    ParseString_ShortNoEscape_Baseline(benchmark::State& state);
void
    ParseString_LongNoEscape(benchmark::State& state);
void
    ParseString_CommonEscapes(benchmark::State& state);
void
    ParseString_UnicodeEscapes(benchmark::State& state);
void
    ParseString_MultibyteUtf8Passthrough(benchmark::State& state);

namespace constants {

constexpr std::string_view kShortInt = "42";
constexpr std::string_view kBlockInt = "12345678";
constexpr std::string_view kLongInt = "1234567890123456789";
constexpr std::string_view kNegativeInt = "-987654321012345";

constexpr std::string_view kSimpleFloat = "3.14";
constexpr std::string_view kExponentFloat = "6.02214076e23";
constexpr std::string_view kSlowPathFloat = "2.2250738585072014e-308";

constexpr std::string_view kTrueLiteral = "true";
constexpr std::string_view kFalseLiteral = "false";
constexpr std::string_view kNullLiteral = "null";

constexpr std::string_view kShortString = R"("hello")";
constexpr std::string_view kLongPlainString =
    R"("this_is_a_much_longer_string_with_no_escapes_at_all_just_ascii_text")";
constexpr std::string_view kEscapedString = R"("line1\nline2\ttabbed\\backslash\"quoted")";
constexpr std::string_view kUnicodeEscapedString = R"("caf\u00e9 \uD83D\uDE00 \u4e2d\u6587")";
constexpr std::string_view kMultibyteUtf8String =
    "\"caf\xC3\xA9 \xE4\xB8\xAD\xE6\x96\x87 \xF0\x9F\x98\x80\"";

} // namespace constants
