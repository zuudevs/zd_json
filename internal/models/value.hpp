/**
 * @file value.hpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Declares Value, the DOM node type for a parsed JSON value.
 * @version 0.4.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#pragma once

#include <cstdint>
#include <expected>
#include <string_view>
#include <type_traits>

#include "enums/json_type.hpp"
#include "zd_json/error.hpp"

namespace zuu::json::models {

class Array;
class Object;

/**
 * @brief A single node in a parsed JSON document tree.
 *
 * Value is a fixed-size, trivially copyable tagged union: it never owns
 * heap memory itself. Array and Object payloads are pointers into the
 * owning Document's Arena, and String payloads are a view -- either into
 * the arena (see Arena::CreateString) or into whatever buffer the caller
 * provided. A Value is therefore only valid for as long as the memory it
 * points into (the owning Document's arena and/or the original input
 * buffer) stays alive.
 *
 * Unlike zuu::json::lexer::Value, which records the raw, not-yet-decoded span
 * of a scanned token, this Value holds the fully decoded, structured
 * result: a real bool, a real double, a nested Array/Object, and so on.
 */
class Value {
  public:
    constexpr Value() noexcept
        : type_(enums::JsonType::Null)
        , bool_(false) {}

    constexpr explicit Value(std::nullptr_t) noexcept
        : type_(enums::JsonType::Null)
        , bool_(false) {}

    constexpr explicit Value(bool value) noexcept
        : type_(enums::JsonType::Bool)
        , bool_(value) {}

    constexpr explicit Value(int64_t value) noexcept
        : type_(enums::JsonType::Integer)
        , int_(value) {}

    constexpr explicit Value(double value) noexcept
        : type_(enums::JsonType::Float)
        , float_(value) {}

    constexpr explicit Value(std::string_view value) noexcept
        : type_(enums::JsonType::String)
        , string_(value) {}

    constexpr explicit Value(Array* value) noexcept
        : type_(enums::JsonType::Array)
        , array_(value) {}

    constexpr explicit Value(Object* value) noexcept
        : type_(enums::JsonType::Object)
        , object_(value) {}

    static constexpr Value
        FromLiteral(int8_t code) noexcept {
        return code != -1 ? Value{static_cast<bool>(code)} : Value{nullptr};
    }

    [[nodiscard]] constexpr enums::JsonType
        type() const noexcept {
        return type_;
    }

    [[nodiscard]] constexpr bool
        is_null() const noexcept {
        return type_ == enums::JsonType::Null;
    }

    [[nodiscard]] constexpr bool
        is_bool() const noexcept {
        return type_ == enums::JsonType::Bool;
    }

    [[nodiscard]] constexpr bool
        is_integer() const noexcept {
        return type_ == enums::JsonType::Integer;
    }

    [[nodiscard]] constexpr bool
        is_float() const noexcept {
        return type_ == enums::JsonType::Float;
    }

    /**
     * @brief True for either numeric representation (Integer or Float).
     */
    [[nodiscard]] constexpr bool
        is_number() const noexcept {
        return is_integer() || is_float();
    }

    [[nodiscard]] constexpr bool
        is_string() const noexcept {
        return type_ == enums::JsonType::String;
    }

    [[nodiscard]] constexpr bool
        is_array() const noexcept {
        return type_ == enums::JsonType::Array;
    }

    [[nodiscard]] constexpr bool
        is_object() const noexcept {
        return type_ == enums::JsonType::Object;
    }

    /**
     * @return The stored bool, or JsonErrc::InvalidType if type() isn't
     *         JsonType::Bool.
     */
    [[nodiscard]] std::expected<bool, JsonErrc>
        as_bool() const noexcept;

    /**
     * @return The stored integer, or JsonErrc::InvalidType if type()
     *         isn't JsonType::Integer.
     */
    [[nodiscard]] std::expected<int64_t, JsonErrc>
        as_integer() const noexcept;

    /**
     * @return The stored float, or JsonErrc::InvalidType if type() isn't
     *         JsonType::Float.
     */
    [[nodiscard]] std::expected<double, JsonErrc>
        as_float() const noexcept;

    /**
     * @brief Reads this value as a double regardless of which numeric
     *        type it holds, widening an Integer if needed.
     *
     * @return The numeric value as a double, or JsonErrc::InvalidType if
     *         is_number() is false.
     */
    [[nodiscard]] std::expected<double, JsonErrc>
        as_double() const noexcept;

    /**
     * @return The stored string view, or JsonErrc::InvalidType if type()
     *         isn't JsonType::String.
     */
    [[nodiscard]] std::expected<std::string_view, JsonErrc>
        as_string() const noexcept;

    /**
     * @return The stored array, or JsonErrc::IsNotArray if type() isn't
     *         JsonType::Array.
     */
    [[nodiscard]] std::expected<const Array*, JsonErrc>
        as_array() const noexcept;

    /// @overload
    [[nodiscard]] std::expected<Array*, JsonErrc>
        as_array() noexcept;

    /**
     * @return The stored object, or JsonErrc::IsNotObject if type() isn't
     *         JsonType::Object.
     */
    [[nodiscard]] std::expected<const Object*, JsonErrc>
        as_object() const noexcept;

    /// @overload
    [[nodiscard]] std::expected<Object*, JsonErrc>
        as_object() noexcept;

  private:
    enums::JsonType type_;
    union {
        bool bool_;
        int64_t int_;
        double float_;
        std::string_view string_;
        Array* array_;
        Object* object_;
    };
};

static_assert(std::is_trivially_copyable_v<Value>, "Value must be trivially copyable.");
static_assert(std::is_trivially_destructible_v<Value>, "Value must be trivially destructible.");

} // namespace zuu::json::models
