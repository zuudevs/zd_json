#include <cassert>
#include <cstring>
#include <zd_json/json_error.hpp>
#include <zd_json/version.hpp>

int
    main() {
    using zuu::JsonErrc;
    using zuu::to_string;

    assert(std::strcmp(to_string(JsonErrc::None), "No error") == 0);
    assert(std::strcmp(to_string(JsonErrc::CommentNotAllowed),
                       "Comments are not allowed in standard JSON") == 0);
    assert(std::strcmp(to_string(JsonErrc::DepthLimitExceeded),
                       "Maximum nesting depth limit exceeded") == 0);
    assert(std::strcmp(to_string(JsonErrc::EmptyValue), "Empty JSON input or value") == 0);
    assert(std::strcmp(to_string(JsonErrc::InvalidBooleanLiteral), "Invalid boolean literal") == 0);
    assert(std::strcmp(to_string(JsonErrc::InvalidFormat), "Invalid JSON format") == 0);
    assert(std::strcmp(to_string(JsonErrc::InvalidNullLiteral), "Invalid null literal") == 0);
    assert(std::strcmp(to_string(JsonErrc::InvalidSurrogate), "Invalid UTF-16 surrogate pair") ==
           0);
    assert(std::strcmp(to_string(JsonErrc::InvalidType), "Invalid or unexpected JSON type") == 0);
    assert(std::strcmp(to_string(JsonErrc::InvalidUnicode), "Invalid UTF-8/Unicode sequence") == 0);
    assert(std::strcmp(to_string(JsonErrc::InvalidValue), "Invalid JSON value") == 0);
    assert(std::strcmp(to_string(JsonErrc::IsNotArray), "JSON element is not an array") == 0);
    assert(std::strcmp(to_string(JsonErrc::IsNotObject), "JSON element is not an object") == 0);
    assert(std::strcmp(to_string(JsonErrc::LeadingZero), "Numbers cannot have leading zeros") == 0);
    assert(std::strcmp(to_string(JsonErrc::MissingComma), "Missing expected comma separator") == 0);
    assert(std::strcmp(to_string(JsonErrc::OutOfBound), "Index or value out of bounds") == 0);
    assert(std::strcmp(to_string(JsonErrc::RootNotArrayType),
                       "Root JSON element is not an array") == 0);
    assert(std::strcmp(to_string(JsonErrc::RootNotObjectType),
                       "Root JSON element is not an object") == 0);
    assert(std::strcmp(to_string(JsonErrc::SingleQuotedString), "Strings must use double quotes") ==
           0);
    assert(std::strcmp(to_string(JsonErrc::TrailingComma), "Trailing commas are not allowed") == 0);
    assert(std::strcmp(to_string(JsonErrc::UnescapedCharacter),
                       "Unescaped control character in string") == 0);
    assert(std::strcmp(to_string(JsonErrc::UnquotedKey),
                       "Object keys must be double-quoted strings") == 0);
    assert(std::strcmp(to_string(JsonErrc::Unknown), "Unknown JSON error") == 0);
    assert(std::strcmp(to_string(static_cast<JsonErrc>(255)), "Unknown JSON error") == 0);

    assert(std::strcmp(zuu::zd_json_version, "0.2.0") == 0);
    assert(zuu::zd_json_version_major == 0);
    assert(zuu::zd_json_version_minor == 2);
    assert(zuu::zd_json_version_patch == 0);

    return 0;
}
