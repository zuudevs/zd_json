/**
 * @file json_serializer_test.hpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Common header for json_serializer unit tests.
 * @version 0.7.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#pragma once

#include <charconv>
#include <cstdint>
#include <cstring>
#include <gtest/gtest.h>
#include <limits>
#include <string>
#include <string_view>

#include "models/arena.hpp"
#include "models/array.hpp"
#include "models/document.hpp"
#include "models/object.hpp"
#include "models/value.hpp"
#include "parser/parse_value.hpp"
#include "serializer/serialize_number.hpp"
#include "serializer/serialize_options.hpp"
#include "serializer/serialize_string.hpp"
#include "serializer/serializer.hpp"

using zuu::json::models::Arena;
using zuu::json::models::Array;
using zuu::json::models::Document;
using zuu::json::models::Object;
using zuu::json::models::Value;
using zuu::json::parser::ParseValue;
using zuu::json::serializer::Serialize;
using zuu::json::serializer::SerializeCompact;
using zuu::json::serializer::SerializeOptions;
using zuu::json::serializer::SerializePretty;
using zuu::json::serializer::WriteEscapedString;
using zuu::json::serializer::WriteFloat;
using zuu::json::serializer::WriteInteger;
