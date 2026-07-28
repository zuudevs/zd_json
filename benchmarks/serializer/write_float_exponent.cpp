/**
 * @file write_float_exponent.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief WriteFloat_Exponent benchmark implementation
 * @version 0.7.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include "json_serializer_benchmarks.hpp"

using namespace constants;

void
    WriteFloat_Exponent(benchmark::State& state) {
    for (auto stateLoop : state) {
        std::string out;
        WriteFloat(out, kExponentFloat);
        benchmark::DoNotOptimize(out);
    }
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
}
