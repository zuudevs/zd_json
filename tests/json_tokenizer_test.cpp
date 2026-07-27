#include <cassert>
#include <iostream>

#include "enums/token_type.hpp"
#include "lexer/tokenizer.hpp"

int
    main() {
    constexpr auto json = R"({"name": "zuu", "age": 20, "skills": ["cpp", "go"]})";

    const auto tokens = zuu::lexer::Tokenize(json);

    assert(!tokens.empty());

    // '{' -> ObjectBeg
    assert(tokens[0].type == static_cast<uint64_t>(zuu::enums::TokenType::ObjectBeg));
    assert(tokens[0].pos == 0);

    // ':' -> Colon
    assert(tokens[1].type == static_cast<uint64_t>(zuu::enums::TokenType::Colon));
    assert(tokens[1].pos == 7);

    // ',' -> Comma
    assert(tokens[2].type == static_cast<uint64_t>(zuu::enums::TokenType::Comma));
    assert(tokens[2].pos == 14);

    // '[' -> ArrayBeg
    assert(tokens[6].type == static_cast<uint64_t>(zuu::enums::TokenType::ArrayBeg));
    assert(tokens[6].pos == 37);

    // ']' -> ArrayEnd
    assert(tokens[8].type == static_cast<uint64_t>(zuu::enums::TokenType::ArrayEnd));
    assert(tokens[8].pos == 49);

    // '}' -> ObjectEnd
    assert(tokens[9].type == static_cast<uint64_t>(zuu::enums::TokenType::ObjectEnd));
    assert(tokens[9].pos == 50);

    for (const auto& token : tokens) {
        std::cout << static_cast<int>(token.type) << " @ " << token.pos << '\n';
    }

    return 0;
}