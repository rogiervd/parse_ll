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
Test whitespace parsers.
*/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE whitespace_parser
#include <boost/test/unit_test.hpp>

#include "parse_ll/core/whitespace.hpp"

#include <type_traits>

#include <string>

#include "range/core.hpp"
#include "range/std/container.hpp"

#include "../helper/fuzz_parser.hpp"

BOOST_AUTO_TEST_SUITE(test_parse_whitespace_parser)

BOOST_AUTO_TEST_CASE (test_whitespace) {
    using range::empty; using range::first; using range::drop;

    using parse_ll::parse;
    using parse_ll::success;
    using parse_ll::output;
    using parse_ll::rest;
    {
        std::string r ("");
        {
            auto parser = parse_ll::space;
            auto result = parse (parser, r);
            BOOST_CHECK (!success (result));
        }
        {
            auto parser = parse_ll::whitespace;
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
        }
        {
            auto parser = parse_ll::newline;
            auto result = parse (parser, r);
            BOOST_CHECK (!success (result));
        }
        {
            auto parser = parse_ll::one_horizontal_space;
            auto result = parse (parser, r);
            BOOST_CHECK (!success (result));
        }
        {
            auto parser = parse_ll::horizontal_space;
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
        }
        {
            auto parser = fuzz (parse_ll::vertical_space);
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
        }
        {
            auto parser = parse_ll::one_whitespace;
            auto result = parse (parser, r);
            BOOST_CHECK (!success (result));
        }
        {
            auto parser = parse_ll::whitespace;
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
        }
    }
    {
        std::string r (" b");
        {
            auto parser = parse_ll::space;
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            static_assert (
                std::is_same <decltype (output (result)), void>::value,
                "Parser should have no output type");
            BOOST_CHECK_EQUAL (first (rest (result)), 'b');
        }
        {
            auto parser = fuzz (parse_ll::one_horizontal_space);
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            static_assert (
                std::is_same <decltype (output (result)), void>::value,
                "Parser should have no output type");
            BOOST_CHECK_EQUAL (first (rest (result)), 'b');
        }
        {
            auto parser = fuzz (parse_ll::horizontal_space);
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            static_assert (
                std::is_same <decltype (output (result)), void>::value,
                "Parser should have no output type");
            BOOST_CHECK_EQUAL (first (rest (result)), 'b');
        }
        {
            auto parser = parse_ll::vertical_space;
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            static_assert (
                std::is_same <decltype (output (result)), void>::value,
                "Parser should have no output type");
            BOOST_CHECK_EQUAL (first (rest (result)), ' ');
        }
        {
            auto parser = parse_ll::one_whitespace;
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            BOOST_CHECK_EQUAL (first (rest (result)), 'b');
        }
        {
            auto parser = parse_ll::whitespace;
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            BOOST_CHECK_EQUAL (first (rest (result)), 'b');
        }
    }
    {
        std::string r ("\r\nc");
        {
            auto parser = parse_ll::carriage_return;
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            BOOST_CHECK_EQUAL (first (rest (result)), '\n');
        }
        {
            auto parser = fuzz (parse_ll::newline);
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            BOOST_CHECK_EQUAL (first (rest (result)), 'c');
        }
        {
            auto parser = parse_ll::one_horizontal_space;
            auto result = parse (parser, r);
            BOOST_CHECK (!success (result));
        }
        {
            auto parser = parse_ll::vertical_space;
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            BOOST_CHECK_EQUAL (first (rest (result)), 'c');
        }
        {
            auto parser = fuzz (parse_ll::whitespace);
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            BOOST_CHECK_EQUAL (first (rest (result)), 'c');
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()

