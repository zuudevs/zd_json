/**
 * @file fuzz_roundtrip.cpp
 * @brief libFuzzer entry point exercising the Parse -> Serialize -> Parse
 *        round trip.
 *
 * Where fuzz_parse.cpp only asks "can Parse() crash on this input?",
 * this target asks a second question: "once zd_json accepts an input,
 * does it stay internally consistent about what that input means?"
 *
 * For every input the fuzzer discovers that Parse() accepts, this
 * harness:
 *
 *   1. Serializes the resulting Document's root twice -- once compact,
 *      once pretty-printed -- exercising both writer code paths
 *      (serializer::SerializeCompact / SerializePretty) against
 *      whatever DOM shape ClusterFuzz/libFuzzer managed to construct.
 *   2. Re-parses both of those outputs. A JSON document produced by
 *      this library's own serializer must always be re-parseable by
 *      this library's own parser: that is the whole point of a
 *      round-trip. A re-parse failure here means Serialize() emitted
 *      something Parse() cannot read back, e.g. a string escaping bug,
 *      a malformed number, or an object/array delimiter mistake.
 *   3. Re-serializes the reparsed document (compact form) and checks it
 *      is byte-for-byte identical to the first compact serialization.
 *      Compact output is the closest thing this library has to a
 *      canonical form, so Parse(Serialize(v)) should serialize back to
 *      exactly the same text -- a mismatch means some information
 *      (precision, structure, escaping) was silently lost or altered
 *      somewhere in the loop.
 *
 * As with fuzz_parse.cpp, inputs Parse() rejects are simply skipped:
 * there is nothing to round-trip. A crash, sanitizer report, or
 * `__builtin_trap()` from a broken invariant below is the finding this
 * target exists to surface.
 */

#include <cstddef>
#include <cstdint>
#include <string_view>

#include "zd_json/json.hpp"

namespace {

// Aborts the process (visible to libFuzzer as a crash, and saved as a
// reproducer) when a round-trip invariant is violated. A plain assert()
// would be compiled out entirely in an NDEBUG fuzzing build, which is
// exactly the build configuration libFuzzer normally runs with -- so the
// check has to be unconditional.
void
    RequireInvariant(bool condition) {
    if (!condition) {
        __builtin_trap();
    }
}

} // namespace

extern "C" int
    LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    const auto* char_data = reinterpret_cast<const char*>(data);
    const std::string_view input(char_data, size);

    auto document = zuu::json::Parse(input);
    if (!document) {
        // Rejected input: nothing to round-trip, and a JsonErrc is a
        // perfectly normal outcome for arbitrary fuzzer bytes.
        return 0;
    }

    const std::string compact_once = zuu::json::SerializeCompact(document->root());
    const std::string pretty_once = zuu::json::SerializePretty(document->root());

    // 1. Whatever this library just serialized, it must be able to read
    //    back -- in both the compact and pretty forms.
    auto reparsed_compact = zuu::json::Parse(compact_once);
    RequireInvariant(reparsed_compact.has_value());

    auto reparsed_pretty = zuu::json::Parse(pretty_once);
    RequireInvariant(reparsed_pretty.has_value());

    // 2. Re-serializing what we just reparsed (compact form) must
    //    reproduce the exact same text: nothing should be lost or
    //    reshuffled by one extra trip through the parser.
    const std::string compact_twice = zuu::json::SerializeCompact(reparsed_compact->root());
    RequireInvariant(compact_twice == compact_once);

    const std::string compact_from_pretty = zuu::json::SerializeCompact(reparsed_pretty->root());
    RequireInvariant(compact_from_pretty == compact_once);

    return 0;
}
