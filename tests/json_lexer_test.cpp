#include <cassert>
#include <iostream>

#include "enums/json_type.hpp"
#include "lexer/lexer.hpp"
#include "lexer/tokenizer.hpp"

int
    main() {
    constexpr auto json =
        R"({"name": "zuu", "age": 20, "score": 98.5, "active": true, "extra": null, "skills": ["cpp", "go"]})";

    const auto tokens = zuu::lexer::tokenize(json);
    const auto values = zuu::lexer::lexer(json, tokens);

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
    const auto tokens_long = zuu::lexer::tokenize(json_long);
    const auto values_long = zuu::lexer::lexer(json_long, tokens_long);

    // "hello\nworld" has escape -> String
    assert(values_long[2].type == zuu::enums::JsonType::String);
    assert(values_long[2].value == R"("hello\nworld")");

    // long string > 16 chars -> String
    assert(values_long[4].type == zuu::enums::JsonType::String);

    std::cout << "All lexer tests passed successfully!\n";

    return 0;
}
