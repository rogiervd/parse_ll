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
Test nothing_parser.
*/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE nothing_parser
#include <boost/test/unit_test.hpp>

#include "parse_ll/core/nothing.hpp"

#include <type_traits>

#include <string>

#include "range/core.hpp"
#include "range/std/container.hpp"

#include "../helper/fuzz_parser.hpp"

BOOST_AUTO_TEST_SUITE(test_parse_nothing)

BOOST_AUTO_TEST_CASE (test_nothing) {
    using range::empty; using range::first; using range::drop;
    {
        std::string s ("");
        {
            auto parser = parse_ll::nothing;
            auto result = parse_ll::parse (parser, s);
            BOOST_CHECK (parse_ll::success (result));
            BOOST_CHECK (empty (parse_ll::rest (result)));
        }
    }
    {
        std::string s ("a");
        {
            auto parser = fuzz (parse_ll::nothing);
            auto result = parse_ll::parse (parser, s);
            // Should be successful, but still not use up any input.
            BOOST_CHECK (parse_ll::success (result));
            static_assert (
                std::is_same <decltype (parse_ll::output (result)), void
                    >::value, "Should output void");
            BOOST_CHECK_EQUAL (first (parse_ll::rest (result)), 'a');
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()

