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
Test difference parser.
*/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE difference_parser
#include <boost/test/unit_test.hpp>

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

