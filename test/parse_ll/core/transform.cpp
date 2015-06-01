/*
Copyright 2012 Rogier van Dalen.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

/** \file
Test transform_parser.
*/

#define BOOST_TEST_MODULE transform_parser
#include "utility/test/boost_unit_test.hpp"

#include "parse_ll/core/transform.hpp"

#include <string>

#include "range/core.hpp"
#include "range/std/container.hpp"

#include "parse_ll/core/nothing.hpp"
#include "parse_ll/core/char.hpp"
#include "../helper/fuzz_parser.hpp"

BOOST_AUTO_TEST_SUITE(test_parse_transform_parser)

struct add {
    int value;

    add (int value) : value (value) {}

    char operator() (char c) const { return char (c + value); }
    int operator() () const { return value; }
};

BOOST_AUTO_TEST_CASE (test_transform) {
    using range::empty; using range::first; using range::drop;

    using parse_ll::parse;
    using parse_ll::success;
    using parse_ll::output;
    using parse_ll::rest;
    {
        std::string r ("");
        {
            auto parser = parse_ll::char_ [add (5)];
            auto result = parse (parser, r);
            BOOST_CHECK (!success (result));
        }
        {
            auto parser = fuzz (parse_ll::nothing [add (5)]);
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            BOOST_CHECK_EQUAL (output (result), 5);
            BOOST_CHECK (empty (rest (result)));
        }
    }
    {
        std::string r ("ab");
        {
            auto parser = fuzz (parse_ll::char_) [add (4)];
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            BOOST_CHECK_EQUAL (output (result), 'e');
            BOOST_CHECK_EQUAL (first (rest (result)), 'b');
        }
        {
            auto parser = fuzz (fuzz (parse_ll::char_('b')) [add (4)]);
            auto result = parse (parser, r);
            BOOST_CHECK (!success (result));
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()
