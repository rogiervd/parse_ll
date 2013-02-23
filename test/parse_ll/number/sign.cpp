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
Test sign parser.
*/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE sign_parser
#include <boost/test/unit_test.hpp>

#include "parse_ll/number/sign.hpp"

#include <string>

#include "range/core.hpp"
#include "range/std/container.hpp"

#include "../helper/fuzz_parser.hpp"

BOOST_AUTO_TEST_SUITE(test_parse_sign)

BOOST_AUTO_TEST_CASE (test_sign) {
    using range::empty; using range::first; using range::drop;

    using parse_ll::parse;
    using parse_ll::success;
    using parse_ll::output;
    using parse_ll::rest;
    {
        std::string r ("");
        {
            auto parser = parse_ll::sign;
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            BOOST_CHECK_EQUAL (output (result), +1);
        }
    }
    {
        std::string r ("0");
        {
            auto parser = parse_ll::sign;
            auto result = parse (parser, r);
            BOOST_CHECK_EQUAL (output (result), +1);
            BOOST_CHECK_EQUAL (first (rest (result)), '0');
        }
    }
    {
        std::string r ("+0");
        {
            auto parser = fuzz (parse_ll::sign);
            {
                auto result = parse (parser, r);
                BOOST_CHECK_EQUAL (output (result), +1);
                BOOST_CHECK_EQUAL (first (rest (result)), '0');
            }
            {
                auto o = output (parse (parser, r));
                BOOST_CHECK_EQUAL (o, +1);
            }
        }
    }
    {
        std::string r ("-0");
        auto parser = fuzz (parse_ll::sign);
        {
            auto result = parse (parser, r);
            BOOST_CHECK_EQUAL (output (result), -1);
            BOOST_CHECK_EQUAL (first (rest (result)), '0');
        }
        {
            auto o = output (parse (parser, r));
            BOOST_CHECK_EQUAL (o, -1);
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()
