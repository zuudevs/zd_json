/**
 * @file fuzz_parse.cpp
 * @brief libFuzzer entry point exercising zuu::json::Parse() with
 *        arbitrary, untrusted byte sequences.
 *
 * This is the primary fuzz target for zd_json: it feeds raw fuzzer input
 * straight into the public Parse() entry point. Parse() is documented as
 * noexcept and returns a Result<Document> (std::expected), so from this
 * harness's point of view there is no "expected" outcome to assert on --
 * any input is legal input. What libFuzzer is actually looking for here,
 * under -fsanitize=fuzzer,address,undefined, is:
 *
 *   - out-of-bounds reads/writes while scanning the input bytes
 *     (tokenizer/lexer/UTF-8 handling, escaped-string decoding, number
 *     parsing, ...),
 *   - use-after-free or double-free in the Arena/Document allocator,
 *   - unsigned/signed overflow, invalid shifts, and other UB the
 *     sanitizers can catch,
 *   - unbounded stack growth from recursive-descent parsing of deeply
 *     nested arrays/objects (guarded by DepthLimitExceeded -- this
 *     target is what proves that guard actually holds under adversarial
 *     nesting),
 *   - crashes/aborts that a hand-written unit test would never think to
 *     try.
 *
 * A crash, a sanitizer report, a timeout, or an OOM found by this target
 * indicates a genuine bug in the library; a JsonErrc failure return is a
 * completely normal, expected outcome and is not itself a finding.
 */

#include <cstddef>
#include <cstdint>
#include <string_view>

#include "zd_json/json.hpp"

extern "C" int
    LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    const auto* char_data = reinterpret_cast<const char*>(data);
    const std::string_view input(char_data, size);

    // The return value is intentionally ignored: both success and every
    // JsonErrc failure are valid, uninteresting outcomes for this
    // target. We only care whether reaching this call ever crashes,
    // hangs, or trips a sanitizer.
    auto result = zuu::json::Parse(input);
    (void)result;

    return 0;
}
