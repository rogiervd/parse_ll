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
Test alternative parser.
*/

#define BOOST_TEST_MODULE alternative_parser
#include "utility/test/boost_unit_test.hpp"

#include "parse_ll/core/alternative.hpp"

#include <string>

#include "range/core.hpp"
#include "range/std/container.hpp"

#include "parse_ll/core/char.hpp"
#include "parse_ll/core/nothing.hpp"
#include "parse_ll/core/transform.hpp"

#include <boost/phoenix/core/value.hpp>

#include "../helper/object.hpp"
#include "../helper/fuzz_parser.hpp"

BOOST_AUTO_TEST_SUITE(test_parse_alternative)


BOOST_AUTO_TEST_CASE (test_alternative) {
    using range::empty; using range::first; using range::drop;

    using parse_ll::parse;
    using parse_ll::success;
    using parse_ll::output;
    using parse_ll::rest;

    using boost::phoenix::val;
    {
        std::string r ("");
        {
            auto parser = parse_ll::char_ ('a') | parse_ll::char_ ('b');
            auto result = parse (parser, r);
            BOOST_CHECK (!success (result));
        }
        {
            // With one sub-parser.
            auto parser = alternative (parse_ll::char_ ('a'));
            auto result = parse (parser, r);
            BOOST_CHECK (!success (result));
        }
        {
            auto parser = alternative (
                parse_ll::char_ ('a'), parse_ll::char_ ('b'));
            auto result = parse (parser, r);
            BOOST_CHECK (!success (result));
        }
        {
            auto parser = parse_ll::char_ ('a') | parse_ll::char_ ('b')
                | fuzz (parse_ll::nothing [boost::phoenix::val ('c')]);
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            BOOST_CHECK_EQUAL (output (result), 'c');
            BOOST_CHECK (empty (rest (result)));
        }
        {
            auto parser = alternative (
                parse_ll::char_ ('a'), parse_ll::char_ ('b'),
                fuzz (parse_ll::nothing [boost::phoenix::val ('c')]));
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            BOOST_CHECK_EQUAL (output (result), 'c');
            BOOST_CHECK (empty (rest (result)));
        }
    }
    {
        std::string r ("ab");
        {
            auto parser = alternative (parse_ll::char_ ('a') [val (1)]);
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            BOOST_CHECK_EQUAL (output (result), 1);
            BOOST_CHECK_EQUAL (first (rest (result)), 'b');
        }
        {
            auto parser =
                parse_ll::char_ ('a') [val (1)]
                | parse_ll::char_ ('b') [val (2)];
            auto result1 = parse (parser, r);
            BOOST_CHECK (success (result1));
            BOOST_CHECK_EQUAL (output (result1), 1);
            auto result2 = parse (parser, rest (result1));
            BOOST_CHECK_EQUAL (output (result2), 2);
            BOOST_CHECK (empty (rest (result2)));
        }
        {
            auto parser =
                alternative (parse_ll::char_ ('a') [val (1)],
                    parse_ll::char_ ('b') [val (2)]);
            auto result1 = parse (parser, r);
            BOOST_CHECK (success (result1));
            BOOST_CHECK_EQUAL (output (result1), 1);
            auto result2 = parse (parser, rest (result1));
            BOOST_CHECK_EQUAL (output (result2), 2);
            BOOST_CHECK (empty (rest (result2)));
        }
        {
            auto parser = fuzz (
                parse_ll::char_ ('b') [val (2)]
                | parse_ll::char_ ('a') [val (1)]);
            {
                auto result1 = parse (parser, r);
                BOOST_CHECK (success (result1));
                BOOST_CHECK_EQUAL (output (result1), 1);
                auto result2 = parse (parser, rest (result1));
                BOOST_CHECK_EQUAL (output (result2), 2);
                BOOST_CHECK (empty (rest (result2)));
                BOOST_CHECK (empty (rest (result2)));
            }
            {
                auto o = output (parse (parser, r));
                BOOST_CHECK_EQUAL (o, 1);
            }
        }
        {
            auto parser = fuzz (alternative (
                parse_ll::char_ ('b') [val (2)],
                parse_ll::char_ ('a') [val (1)]));
            {
                auto result1 = parse (parser, r);
                BOOST_CHECK (success (result1));
                BOOST_CHECK_EQUAL (output (result1), 1);
                auto result2 = parse (parser, rest (result1));
                BOOST_CHECK_EQUAL (output (result2), 2);
                BOOST_CHECK (empty (rest (result2)));
            }
            {
                auto o = output (parse (parser, r));
                BOOST_CHECK_EQUAL (o, 1);
            }
        }
        {
            // The first parser that match should be used.
            auto parser = fuzz (
                fuzz (parse_ll::char_ ('b') [always (object (2))])
                | parse_ll::char_ ('a') [always (object (1))]
                | parse_ll::nothing [always (object (3))]);
            {
                auto result1 = parse (parser, r);
                BOOST_CHECK (success (result1));
                BOOST_CHECK_EQUAL (output (result1), 1);
                auto result2 = parse (parser, rest (result1));
                BOOST_CHECK_EQUAL (output (result2), 2);
            }
            {
                auto o = output (parse (parser, r));
                BOOST_CHECK_EQUAL (o, 1);
            }
        }
        {
            auto parser = fuzz (alternative (
                fuzz (parse_ll::char_ ('b') [always (object (2))]),
                parse_ll::char_ ('a') [always (object (1))],
                parse_ll::nothing [always (object (3))]));
            {
                auto result1 = parse (parser, r);
                BOOST_CHECK (success (result1));
                BOOST_CHECK_EQUAL (output (result1), 1);
                auto result2 = parse (parser, rest (result1));
                BOOST_CHECK_EQUAL (output (result2), 2);
            }
            {
                auto o = output (parse (parser, r));
                BOOST_CHECK_EQUAL (o, 1);
            }
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()

