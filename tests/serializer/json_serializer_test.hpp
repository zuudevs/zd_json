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

#include <gtest/gtest.h>
#include <charconv>
#include <cstdint>
#include <cstring>
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

using zuu::models::Arena;
using zuu::models::Array;
using zuu::models::Document;
using zuu::models::Object;
using zuu::models::Value;
using zuu::parser::ParseValue;
using zuu::serializer::Serialize;
using zuu::serializer::SerializeCompact;
using zuu::serializer::SerializeOptions;
using zuu::serializer::SerializePretty;
using zuu::serializer::WriteEscapedString;
using zuu::serializer::WriteFloat;
using zuu::serializer::WriteInteger;
