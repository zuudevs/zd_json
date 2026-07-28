/**
 * @file value.cpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Implements Value's typed accessors.
 * @version 0.4.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#include "models/value.hpp"

namespace zuu::json::models {

std::expected<bool, JsonErrc>
    Value::as_bool() const noexcept {
    if (type_ != enums::JsonType::Bool) {
        return std::unexpected{JsonErrc::InvalidType};
    }
    return bool_;
}

std::expected<int64_t, JsonErrc>
    Value::as_integer() const noexcept {
    if (type_ != enums::JsonType::Integer) {
        return std::unexpected{JsonErrc::InvalidType};
    }
    return int_;
}

std::expected<double, JsonErrc>
    Value::as_float() const noexcept {
    if (type_ != enums::JsonType::Float) {
        return std::unexpected{JsonErrc::InvalidType};
    }
    return float_;
}

std::expected<double, JsonErrc>
    Value::as_double() const noexcept {
    switch (type_) {
        case enums::JsonType::Float:
            return float_;
        case enums::JsonType::Integer:
            return static_cast<double>(int_);
        default:
            return std::unexpected{JsonErrc::InvalidType};
    }
}

std::expected<std::string_view, JsonErrc>
    Value::as_string() const noexcept {
    if (type_ != enums::JsonType::String) {
        return std::unexpected{JsonErrc::InvalidType};
    }
    return string_;
}

std::expected<const Array*, JsonErrc>
    Value::as_array() const noexcept {
    if (type_ != enums::JsonType::Array) {
        return std::unexpected{JsonErrc::IsNotArray};
    }
    return array_;
}

std::expected<Array*, JsonErrc>
    Value::as_array() noexcept {
    if (type_ != enums::JsonType::Array) {
        return std::unexpected{JsonErrc::IsNotArray};
    }
    return array_;
}

std::expected<const Object*, JsonErrc>
    Value::as_object() const noexcept {
    if (type_ != enums::JsonType::Object) {
        return std::unexpected{JsonErrc::IsNotObject};
    }
    return object_;
}

std::expected<Object*, JsonErrc>
    Value::as_object() noexcept {
    if (type_ != enums::JsonType::Object) {
        return std::unexpected{JsonErrc::IsNotObject};
    }
    return object_;
}

} // namespace zuu::json::models
