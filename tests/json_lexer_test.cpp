#include <cassert>
#include <iostream>
#include <string>
#include <string_view>

#include "enums/json_type.hpp"
#include "lexer/lexer.hpp"
#include "lexer/tokenizer.hpp"
#include "zd_json/json_error.hpp"

int
    main() {
    constexpr auto json =
        R"({"name": "zuu", "age": 20, "score": 98.5, "active": true, "extra": null, "skills": ["cpp", "go"]})";

    const auto tokens = zuu::lexer::Tokenize(json);
    const auto values = zuu::lexer::LexValues(json, tokens);

    assert(!values.empty());

    // 0: Object '{'
    assert(values[0].type == zuu::enums::JsonType::Object);
    assert(values[0].value == "{");

    // 1: Key "name" -> Short
    assert(values[1].type == zuu::enums::JsonType::Short);
    assert(values[1].value == R"("name")");

    // 2: Value "zuu" -> Short
    assert(values[2].type == zuu::enums::JsonType::Short);
    assert(values[2].value == R"("zuu")");

    // 3: Key "age" -> Short
    assert(values[3].type == zuu::enums::JsonType::Short);
    assert(values[3].value == R"("age")");

    // 4: Value 20 -> Integer
    assert(values[4].type == zuu::enums::JsonType::Integer);
    assert(values[4].value == "20");

    // 5: Key "score" -> Short
    assert(values[5].type == zuu::enums::JsonType::Short);
    assert(values[5].value == R"("score")");

    // 6: Value 98.5 -> Float
    assert(values[6].type == zuu::enums::JsonType::Float);
    assert(values[6].value == "98.5");

    // 7: Key "active" -> Short
    assert(values[7].type == zuu::enums::JsonType::Short);
    assert(values[7].value == R"("active")");

    // 8: Value true -> Bool
    assert(values[8].type == zuu::enums::JsonType::Bool);
    assert(values[8].value == "true");

    // 9: Key "extra" -> Short
    assert(values[9].type == zuu::enums::JsonType::Short);
    assert(values[9].value == R"("extra")");

    // 10: Value null -> Null
    assert(values[10].type == zuu::enums::JsonType::Null);
    assert(values[10].value == "null");

    // 11: Key "skills" -> Short
    assert(values[11].type == zuu::enums::JsonType::Short);
    assert(values[11].value == R"("skills")");

    // 12: Array '[' -> Array
    assert(values[12].type == zuu::enums::JsonType::Array);
    assert(values[12].value == "[");

    // 13: "cpp" -> Short
    assert(values[13].type == zuu::enums::JsonType::Short);
    assert(values[13].value == R"("cpp")");

    // 14: "go" -> Short
    assert(values[14].type == zuu::enums::JsonType::Short);
    assert(values[14].value == R"("go")");

    // Test long string and escaped string
    constexpr auto json_long =
        R"({"escaped": "hello\nworld", "long": "this_is_a_very_long_string_exceeding_sixteen_chars"})";
    const auto tokens_long = zuu::lexer::Tokenize(json_long);
    const auto values_long = zuu::lexer::LexValues(json_long, tokens_long);

    // "hello\nworld" has escape -> String
    assert(values_long[2].type == zuu::enums::JsonType::String);
    assert(values_long[2].value == R"("hello\nworld")");

    // long string > 16 chars -> String
    assert(values_long[4].type == zuu::enums::JsonType::String);

    // Valid documents: every scanned Value reports no validation error.
    for (const auto& v : values) {
        assert(v.error == zuu::JsonErrc::None);
    }
    for (const auto& v : values_long) {
        assert(v.error == zuu::JsonErrc::None);
    }

    // --- UTF-8 / control-character validation (ScanString's SWAR pass) ---

    // Well-formed 2/3/4-byte UTF-8 sequences.
    {
        constexpr auto s = "\"caf\xC3\xA9 \xE4\xB8\xAD \xF0\x9F\x98\x80\"";
        std::string_view sv(s);
        const auto result = zuu::lexer::ScanString(sv, 0, sv.size());
        assert(result.error == zuu::JsonErrc::None);
        assert(result.value_end == sv.size());
    }

    // Invalid continuation byte after a 2-byte lead.
    {
        constexpr auto s = "\"\xC3"
                           "\x28"
                           "bad\"";
        std::string_view sv(s);
        const auto result = zuu::lexer::ScanString(sv, 0, sv.size());
        assert(result.error == zuu::JsonErrc::InvalidUnicode);
    }

    // Overlong 2-byte encoding of U+0000 (0xC0 0x80).
    {
        constexpr auto s = "\"\xC0\x80\"";
        std::string_view sv(s);
        const auto result = zuu::lexer::ScanString(sv, 0, sv.size());
        assert(result.error == zuu::JsonErrc::InvalidUnicode);
    }

    // A UTF-16 surrogate value (U+D800) encoded directly as UTF-8 bytes.
    {
        constexpr auto s = "\"\xED\xA0\x80\"";
        std::string_view sv(s);
        const auto result = zuu::lexer::ScanString(sv, 0, sv.size());
        assert(result.error == zuu::JsonErrc::InvalidSurrogate);
    }

    // Unescaped raw control character.
    {
        std::string s = "\"";
        s.push_back('\x09');
        s += "\"";
        std::string_view sv(s);
        const auto result = zuu::lexer::ScanString(sv, 0, sv.size());
        assert(result.error == zuu::JsonErrc::UnescapedCharacter);
    }

    // An ASCII run spanning multiple 8-byte SWAR blocks scans cleanly.
    {
        std::string s = "\"" + std::string(40, 'a') + "\"";
        std::string_view sv(s);
        const auto result = zuu::lexer::ScanString(sv, 0, sv.size());
        assert(result.error == zuu::JsonErrc::None);
        assert(result.value_end == sv.size());
    }

    // The error is still visible through the full LexValues pipeline.
    {
        constexpr auto s = "\"\xC0\x80\"";
        const auto invalid_tokens = zuu::lexer::Tokenize(s);
        const auto invalid_values = zuu::lexer::LexValues(s, invalid_tokens);
        assert(invalid_values.size() == 1);
        assert(invalid_values[0].error == zuu::JsonErrc::InvalidUnicode);
    }

    std::cout << "All lexer tests passed successfully!\n";

    return 0;
}
