# Roadmap

## v0.1.0 Project Foundation

* [x] Initialize project structure
* [x] Configure CMake
* [x] Integrate vcpkg
* [x] Configure Clang/LLVM toolchain
* [x] Add clang-format
* [x] Add clang-tidy
* [x] Configure sanitizers
* [x] Configure code coverage
* [x] Configure GitHub Actions

---

## v0.2.0 Core

* [x] Implement `ErrorCode`
* [x] Implement `Error`
* [x] Implement `Result<T>` using `std::expected`
* [x] Implement version information
* [x] Design public API

---

## v0.3.0 Lexer

* [x] Structural character scanner
* [x] Parse string tokens
* [x] Parse number tokens
* [x] Parse boolean tokens
* [x] Parse null tokens
* [x] UTF-8 validation

---

## v0.4.0 DOM

* [x] Implement `Value`
* [x] Implement `Array`
* [x] Implement `Object`
* [x] Implement `Document`

---

## v0.5.0 Parser

* [x] Parse objects
* [x] Parse arrays
* [x] Parse strings
* [x] Parse numbers
* [x] Parse literals (`true`, `false`, `null`)
* [ ] Improve parser diagnostics

---

## v0.6.0 Serializer

* [x] Compact JSON writer
* [x] Pretty JSON writer

---

## v0.7.0 Testing

* [x] Unit tests
* [x] Integration tests
* [x] Fuzz testing

---

## v0.8.0 Benchmark

* [x] Integrate Google Benchmark
* [x] Add parsing benchmarks
* [x] Add serialization benchmarks
* [x] Add DOM benchmarks
* [x] Benchmark against real-world datasets

---

## v0.9.0 Benchmark Reporter

* [x] Save benchmark history
* [x] Compare benchmark results
* [x] Detect performance regression
* [x] Generate Markdown report
* [x] Generate charts

---

## v0.10.0 Performance

* [ ] Reduce memory allocations
* [ ] Optimize string parsing
* [ ] Optimize number parsing
* [ ] Optimize UTF-8 validation

---

## v0.11.0 Documentation

* [ ] API documentation
* [ ] Architecture guide
* [ ] Benchmark guide
* [ ] Examples

---

## v1.0.0 Stable Release

- [ ] Public API stabilization
- [ ] Complete test coverage
- [ ] Performance baseline established
- [ ] Documentation completed
- [ ] Cross-platform validation
- [ ] First stable release

---

## Future

* [ ] Add SIMD backend
* [ ] On-Demand API
* [ ] SAX parser
* [ ] JSON Pointer (RFC 6901)
* [ ] JSON Patch (RFC 6902)
* [ ] Custom allocator
* [ ] C++ Modules
