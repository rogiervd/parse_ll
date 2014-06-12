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
Test digit parser.
*/

#define BOOST_TEST_MODULE digit_parser
#include "utility/test/boost_unit_test.hpp"

#include "parse_ll/number/digit.hpp"

#include <string>

#include "range/core.hpp"
#include "range/std/container.hpp"

#include "../helper/fuzz_parser.hpp"

BOOST_AUTO_TEST_SUITE(test_parse_digit)

BOOST_AUTO_TEST_CASE (test_digit) {
    using range::empty;

    using parse_ll::parse;
    using parse_ll::success;
    using parse_ll::output;
    using parse_ll::rest;
    {
        std::string r ("");
        {
            auto parser = parse_ll::digit;
            auto result = parse (parser, r);
            BOOST_CHECK (!success (result));
        }
    }
    {
        std::string r ("a");
        {
            auto parser = fuzz (parse_ll::digit);
            auto result = parse (parser, r);
            BOOST_CHECK (!success (result));
        }
    }
    {
        std::string r ("017");
        {
            auto parser = parse_ll::digit;
            auto result1 = parse (parser, r);
            BOOST_CHECK (success (result1));
            BOOST_CHECK_EQUAL (output (result1), 0);
            BOOST_CHECK (!empty (rest (result1)));

            auto result2 = parse (parser, rest (result1));
            BOOST_CHECK (success (result2));
            BOOST_CHECK_EQUAL (output (result2), 1);
            BOOST_CHECK (!empty (rest (result2)));

            auto result3 = parse (parser, rest (result2));
            BOOST_CHECK (success (result3));
            BOOST_CHECK_EQUAL (output (result3), 7);
            BOOST_CHECK (empty (rest (result3)));

            auto result4 = parse (parser, rest (result3));
            BOOST_CHECK (!success (result4));
        }
    }
    {
        std::string r ("9");
        {
            auto parser = fuzz (parse_ll::digit);
            {
                auto result = parse (parser, r);
                BOOST_CHECK (success (result));
                BOOST_CHECK_EQUAL (output (result), 9);
                BOOST_CHECK (empty (rest (result)));
            }
            {
                auto o = output (parse (parser, r));
                BOOST_CHECK_EQUAL (o, 9);
            }
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()
