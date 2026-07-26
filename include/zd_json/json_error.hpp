/**
 * @file json_error.hpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Brief description
 * @version 0.2.0
 * @date 2026-07-26
 *
 * @copyright Copyright (c) 2026
 */

#pragma once

#include <cstdint>

namespace zuu {

enum class JsonErrc : uint8_t {
    None,

    CommentNotAllowed,
    DepthLimitExceeded,
    EmptyValue,
    InvalidBooleanLiteral,
    InvalidFormat,
    InvalidNullLiteral,
    InvalidSurrogate,
    InvalidType,
    InvalidUnicode,
    InvalidValue,
    IsNotArray,
    IsNotObject,
    LeadingZero,
    MissingComma,
    OutOfBound,
    RootNotArrayType,
    RootNotObjectType,
    SingleQuotedString,
    TrailingComma,
    UnescapedCharacter,
    UnquotedKey,

    Unknown
};

[[nodiscard]] constexpr const char*
    to_string(JsonErrc code) noexcept {
    switch (code) {
        case JsonErrc::None:
            return "No error";
        case JsonErrc::CommentNotAllowed:
            return "Comments are not allowed in standard JSON";
        case JsonErrc::DepthLimitExceeded:
            return "Maximum nesting depth limit exceeded";
        case JsonErrc::EmptyValue:
            return "Empty JSON input or value";
        case JsonErrc::InvalidBooleanLiteral:
            return "Invalid boolean literal";
        case JsonErrc::InvalidFormat:
            return "Invalid JSON format";
        case JsonErrc::InvalidNullLiteral:
            return "Invalid null literal";
        case JsonErrc::InvalidSurrogate:
            return "Invalid UTF-16 surrogate pair";
        case JsonErrc::InvalidType:
            return "Invalid or unexpected JSON type";
        case JsonErrc::InvalidUnicode:
            return "Invalid UTF-8/Unicode sequence";
        case JsonErrc::InvalidValue:
            return "Invalid JSON value";
        case JsonErrc::IsNotArray:
            return "JSON element is not an array";
        case JsonErrc::IsNotObject:
            return "JSON element is not an object";
        case JsonErrc::LeadingZero:
            return "Numbers cannot have leading zeros";
        case JsonErrc::MissingComma:
            return "Missing expected comma separator";
        case JsonErrc::OutOfBound:
            return "Index or value out of bounds";
        case JsonErrc::RootNotArrayType:
            return "Root JSON element is not an array";
        case JsonErrc::RootNotObjectType:
            return "Root JSON element is not an object";
        case JsonErrc::SingleQuotedString:
            return "Strings must use double quotes";
        case JsonErrc::TrailingComma:
            return "Trailing commas are not allowed";
        case JsonErrc::UnescapedCharacter:
            return "Unescaped control character in string";
        case JsonErrc::UnquotedKey:
            return "Object keys must be double-quoted strings";
        case JsonErrc::Unknown:
        default:
            return "Unknown JSON error";
    }
}

} // namespace zuu