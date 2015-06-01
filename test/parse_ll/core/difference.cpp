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
Test difference parser.
*/

#define BOOST_TEST_MODULE difference_parser
#include "utility/test/boost_unit_test.hpp"

#include "parse_ll/core/difference.hpp"

#include <string>

#include "range/core.hpp"
#include "range/std/container.hpp"

#include "parse_ll/core/char.hpp"
#include "parse_ll/core/literal.hpp"
#include "parse_ll/core/transform.hpp"
#include "parse_ll/core/repeat.hpp"

#include <boost/phoenix/core/value.hpp>
#include "../helper/fuzz_parser.hpp"

BOOST_AUTO_TEST_SUITE(test_parse_difference)


BOOST_AUTO_TEST_CASE (test_difference) {
    using range::empty; using range::first; using range::drop;

    using parse_ll::parse;
    using parse_ll::success;
    using parse_ll::output;
    using parse_ll::rest;

    using boost::phoenix::val;
    {
        std::string r ("");
        {
            auto parser = parse_ll::char_ ('a') - parse_ll::char_ ('b');
            auto result = parse (parser, r);
            BOOST_CHECK (!success (result));
        }
    }
    {
        std::string r ("ab");
        {
            auto parser =
                fuzz (fuzz (parse_ll::char_ ('a') [val (1)])
                    - fuzz (parse_ll::char_ ('b')));
            auto result1 = parse (parser, r);
            BOOST_CHECK (success (result1));
            BOOST_CHECK_EQUAL (output (result1), 1);
            auto result2 = parse (parser, rest (result1));
            BOOST_CHECK (!success (result2));
        }
    }
    {
        std::string r ("45 a");
        {
            auto parser =
                fuzz (*fuzz (parse_ll::char_ - parse_ll::literal (' ')));
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            BOOST_CHECK_EQUAL (first (output (result)), '4');
            BOOST_CHECK_EQUAL (first (drop (output (result))), '5');
            BOOST_CHECK (::range::empty (drop (drop (output (result)))));
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()

