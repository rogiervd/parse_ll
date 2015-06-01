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
Test literal_parser.
*/

#define BOOST_TEST_MODULE literal_parser
#include "utility/test/boost_unit_test.hpp"

#include "parse_ll/core/literal.hpp"

#include <type_traits>
#include <string>

#include "range/core.hpp"
#include "range/std/container.hpp"

#include "../helper/fuzz_parser.hpp"

BOOST_AUTO_TEST_SUITE(test_parse_literal_parser)

BOOST_AUTO_TEST_CASE (test_literal) {
    using range::empty; using range::first; using range::drop;

    using parse_ll::parse;
    using parse_ll::success;
    using parse_ll::output;
    using parse_ll::rest;

    using parse_ll::literal;
    {
        std::string r ("");
        {
            auto parser = literal ('s');
            auto result = parse (parser, r);
            BOOST_CHECK (!success (result));
            static_assert (
                std::is_same <decltype (output (result)), void>::value,
                    "lit should not have a void output");
        }
        {
            auto parser = literal ("st");
            auto result = parse (parser, r);
            BOOST_CHECK (!success (result));
        }
        {
            auto parser = fuzz (literal (""));
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
        }
    }
    {
        std::string r ("ab");
        {
            auto parser = literal ('a');
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            BOOST_CHECK_EQUAL (first (rest (result)), 'b');
        }
        {
            auto parser = literal ('b');
            auto result = parse (parser, r);
            BOOST_CHECK (!success (result));
        }
        {
            auto parser = fuzz (literal ("a"));
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            BOOST_CHECK_EQUAL (first (rest (result)), 'b');
        }
        {
            auto parser = fuzz (literal ("ab"));
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            BOOST_CHECK (empty (rest (result)));
        }
    }
    {
        std::string r ("aab5");

        {
            auto parser = literal ("aa");
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            BOOST_CHECK_EQUAL (first (rest (result)), 'b');
        }
        {
            auto parser = fuzz (literal ("ab"));
            auto result = parse (parser, r);
            BOOST_CHECK (!success (result));
        }
        {
            auto parser = literal ("aab");
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            BOOST_CHECK_EQUAL (first (rest (result)), '5');
        }
        {
            auto parser = fuzz (literal ("aab5"));
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            BOOST_CHECK (empty (rest (result)));
        }
        {
            auto parser = literal ("aab55");
            auto result = parse (parser, r);
            BOOST_CHECK (!success (result));
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()
