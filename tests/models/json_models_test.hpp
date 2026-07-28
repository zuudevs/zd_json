/**
 * @file json_models_test.hpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Common header for json_models unit tests.
 * @version 0.7.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#pragma once

#include <cstring>
#include <gtest/gtest.h>
#include <string_view>

#include "enums/json_type.hpp"
#include "models/arena.hpp"
#include "models/array.hpp"
#include "models/document.hpp"
#include "models/object.hpp"
#include "models/value.hpp"
#include "zd_json/error.hpp"

using zuu::json::JsonErrc;
using zuu::json::enums::JsonType;
using zuu::json::models::Arena;
using zuu::json::models::Array;
using zuu::json::models::Document;
using zuu::json::models::Object;
using zuu::json::models::Value;
