# Fuzz testing (LLVM libFuzzer)

This directory holds zd_json's coverage-guided fuzz targets, built on
[LLVM's libFuzzer](https://llvm.org/docs/LibFuzzer.html). They complement
`tests/unit` and `tests/integration` (which check known cases) by
throwing adversarial, mutated byte sequences at the public API and
relying on AddressSanitizer/UndefinedBehaviorSanitizer to catch memory
and UB bugs that hand-written tests would never think to try.

## Layout

```
tests/fuzz/
├── fuzz_parse.cpp          # Target 1: raw bytes -> zuu::json::Parse()
├── fuzz_roundtrip.cpp      # Target 2: Parse -> Serialize -> Parse consistency
├── corpus/
│   ├── fuzz_parse/         # Seed inputs for fuzz_parse
│   └── fuzz_roundtrip/     # Seed inputs for fuzz_roundtrip (valid JSON only)
├── dictionaries/
│   └── json.dict           # libFuzzer token dictionary for the JSON grammar
└── README.md
```

### `fuzz_parse`

Feeds raw fuzzer bytes directly into `zuu::json::Parse()`. Every input is
a legal input from the harness's point of view -- `Parse()` is
`noexcept` and returns `Result<Document>`, so a `JsonErrc` failure is a
normal, uninteresting outcome. What this target is actually looking for
is anything a fuzzer input can make the library do that a `JsonErrc`
can't express: out-of-bounds reads in the tokenizer/lexer/UTF-8
handling, arena use-after-free, integer overflow UB in number parsing,
unbounded recursion past `DepthLimitExceeded`, aborts, hangs, etc.

### `fuzz_roundtrip`

For every input `fuzz_parse` would find *interesting* -- i.e. every
input `Parse()` actually accepts -- this target checks a structural
invariant: `Serialize(Parse(text))` must always be something
`Parse()` can read back, and re-serializing what comes back out must
reproduce the exact same compact text. This is where serializer bugs
(bad escaping, lossy number formatting, malformed delimiters) show up,
as opposed to parser-only crashes.

## Building

libFuzzer is a Clang/compiler-rt feature (`-fsanitize=fuzzer`) with no
GCC or MSVC equivalent, so these targets are opt-in and require Clang:

```sh
cmake -B out/fuzz -DCMAKE_CXX_COMPILER=clang++ -DZD_JSON_BUILD_FUZZERS=ON
cmake --build out/fuzz --target zd_fuzz_parse zd_fuzz_roundtrip
```

Configuring with `ZD_JSON_BUILD_FUZZERS=ON` under a non-Clang compiler
fails the configure step with a clear error instead of silently
skipping the targets.

## Running

```sh
# Fuzz indefinitely, seeded from and growing the on-disk corpus:
./out/fuzz/bin/zd_fuzz_parse tests/fuzz/corpus/fuzz_parse \
    -dict=tests/fuzz/dictionaries/json.dict

./out/fuzz/bin/zd_fuzz_roundtrip tests/fuzz/corpus/fuzz_roundtrip \
    -dict=tests/fuzz/dictionaries/json.dict

# Useful flags:
#   -max_total_time=300   stop after 5 minutes (good for CI/local smoke runs)
#   -jobs=8 -workers=8    run 8 fuzzing processes in parallel
#   -max_len=65536         cap generated input size
```

Each target is also registered as a CTest case
(`zd_fuzz_parse_corpus_replay`, `zd_fuzz_roundtrip_corpus_replay`) that
replays the checked-in seed corpus once (`-runs=0 <corpus_dir>`) with
sanitizers on. That's a cheap regression check that the harnesses and
seeds themselves stay crash-free -- it is **not** a substitute for
actually running the fuzzer; it doesn't generate any new inputs.

```sh
ctest --test-dir out/fuzz -R zd_fuzz
```

## Reproducing and fixing a crash

A crashing input gets written to the working directory as
`crash-<sha1>`. Reproduce and debug it directly:

```sh
./out/fuzz/bin/zd_fuzz_parse crash-<sha1>
```

Once fixed, minimize the crashing input and add it to the seed corpus so
it's replayed by CI forever after:

```sh
./out/fuzz/bin/zd_fuzz_parse -minimize_crash=1 -runs=10000 crash-<sha1>
cp minimized-from-* tests/fuzz/corpus/fuzz_parse/
```

## Growing the corpus

After a fuzzing session, merge any new coverage-increasing inputs it
found back into the checked-in seed corpus:

```sh
./out/fuzz/bin/zd_fuzz_parse -merge=1 \
    tests/fuzz/corpus/fuzz_parse /path/to/session/findings
```

Keep the checked-in corpus reasonably small and diverse (one input per
interesting code path) rather than dumping an entire fuzzing session's
output into version control.
