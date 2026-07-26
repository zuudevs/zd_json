/**
 * @file errc.hpp
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

enum class JsonError : uint8_t {
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

} // namespace zuu