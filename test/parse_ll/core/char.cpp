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
Test char_parser.
*/

#define BOOST_TEST_MODULE char_parser
#include "utility/test/boost_unit_test.hpp"

#include "parse_ll/core/char.hpp"

#include <type_traits>

#include <string>

#include "range/core.hpp"
#include "range/std/container.hpp"

#include "../helper/fuzz_parser.hpp"

BOOST_AUTO_TEST_SUITE(test_parse_char_parser)


BOOST_AUTO_TEST_CASE (test_char) {
    using range::empty; using range::first; using range::drop;

    using parse_ll::parse;
    using parse_ll::success;
    using parse_ll::output;
    using parse_ll::rest;
    {
        std::string r ("");
        {
            auto parser = parse_ll::char_;
            auto result = parse (parser, r);
            BOOST_CHECK (!success (result));
        }
        {
            auto parser = parse_ll::char_ ('1');
            auto result = parse (parser, r);
            BOOST_CHECK (!success (result));
        }
    }
    {
        std::string r ("ab");
        {
            auto parser = fuzz (parse_ll::char_);
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            BOOST_CHECK_EQUAL (output (result), 'a');
            BOOST_CHECK_EQUAL (first (rest (result)), 'b');
        }
        {
            auto parser = parse_ll::char_('b');
            auto result = parse (parser, r);
            BOOST_CHECK (!success (result));
        }
        {
            auto parser = fuzz (parse_ll::char_('a'));
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            BOOST_CHECK_EQUAL (output (result), 'a');
            BOOST_CHECK_EQUAL (first (rest (result)), 'b');
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()
