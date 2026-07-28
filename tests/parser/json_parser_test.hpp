/**
 * @file json_parser_test.hpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Common header for json_parser unit tests.
 * @version 0.7.0
 * @date 2026-07-28
 *
 * @copyright Copyright (c) 2026
 */

#pragma once

#include <gtest/gtest.h>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <string>

#include "models/arena.hpp"
#include "models/array.hpp"
#include "models/object.hpp"
#include "models/value.hpp"
#include "parser/parse_array.hpp"
#include "parser/parse_bool.hpp"
#include "parser/parse_float.hpp"
#include "parser/parse_integral.hpp"
#include "parser/parse_literal.hpp"
#include "parser/parse_null.hpp"
#include "parser/parse_object.hpp"
#include "parser/parse_string.hpp"

using zuu::JsonErrc;
using zuu::models::Arena;
using zuu::models::Array;
using zuu::models::Object;
using zuu::models::Value;
using zuu::parser::ParseArray;
using zuu::parser::ParseBool;
using zuu::parser::ParseFloat;
using zuu::parser::ParseIntegral;
using zuu::parser::ParseLiteral;
using zuu::parser::ParseNull;
using zuu::parser::ParseObject;
using zuu::parser::ParseShortString;
using zuu::parser::ParseString;

inline bool
    approx_equal(double a, double b) {
    if (a == b) {
        return true;
    }
    const double diff = std::fabs(a - b);
    const double scale = std::max(std::fabs(a), std::fabs(b));
    return diff <= scale * 1e-12;
}
