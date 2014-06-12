/*
Copyright 2012 Rogier van Dalen.

This file is part of Rogier van Dalen's LL Parser library for C++.

This library is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
