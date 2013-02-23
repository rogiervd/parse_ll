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
Test named parsers.
*/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE named_parser
#include <boost/test/unit_test.hpp>

#include "parse_ll/core/named.hpp"

#include <string>

#include "range/core.hpp"
#include "range/std/container.hpp"

#include "parse_ll/core/char.hpp"
#include "parse_ll/core/repeat.hpp"

#include "../helper/fuzz_parser.hpp"

using parse_ll::parse;
using parse_ll::success;
using parse_ll::output;
using parse_ll::rest;

using parse_ll::char_;

PARSE_LL_DEFINE_NAMED_PARSER_TYPE ("name1", name1_parser,
    *char_ ('a'));
PARSE_LL_DEFINE_NAMED_PARSER (name2,
    *char_ ('a'));
PARSE_LL_DEFINE_NAMED_PARSER_TEMPLATE ("name3", name3_parser, Result,
    *char_ (Result ('a')));

BOOST_AUTO_TEST_SUITE(test_parse_named)

template <class Parser> void check_repeat_a (Parser const & parser) {
    using range::empty; using range::first; using range::drop;
    {
        std::string r ("aa");
        auto result = parse (parser, r);
        BOOST_CHECK (success (result));
        auto o = output (result);
        BOOST_CHECK (!empty (o));
        BOOST_CHECK (!empty (drop (o)));
        BOOST_CHECK (empty (drop (drop (o))));
        BOOST_CHECK_EQUAL (first (o), 'a');
        BOOST_CHECK_EQUAL (first (drop (o)), 'a');
        BOOST_CHECK (empty (rest (result)));
    }
    {
        std::string r ("ab");
        auto result = parse (parser, r);
        BOOST_CHECK (success (result));
        auto o = output (result);
        BOOST_CHECK (!empty (o));
        BOOST_CHECK (empty (drop (o)));
        BOOST_CHECK_EQUAL (first (o), 'a');
        BOOST_CHECK_EQUAL (first (rest (result)), 'b');
    }
    {
        std::string r ("b");
        auto result = parse (parser, r);
        BOOST_CHECK (success (result));
        auto o = output (result);
        BOOST_CHECK (empty (o));
        BOOST_CHECK_EQUAL (first (rest (result)), 'b');
    }
}

template <class Parser> void check_repeat_a_fuzz (Parser const & parser) {
    check_repeat_a (parser);
    check_repeat_a (fuzz (parser));
}

// In class scope.
PARSE_LL_DEFINE_NAMED_PARSER_TYPE ("name4", name4_parser,
    *char_ ('a'));
PARSE_LL_DEFINE_NAMED_PARSER (name5,
    *char_ ('a'));
PARSE_LL_DEFINE_NAMED_PARSER_TEMPLATE ("name6", name6_parser, Result,
    *char_ (Result ('a')));

BOOST_AUTO_TEST_CASE (test_named) {
    check_repeat_a_fuzz (name1_parser());
    check_repeat_a_fuzz (name2);
    check_repeat_a_fuzz (name3_parser <char>());
    check_repeat_a_fuzz (name4_parser());
    check_repeat_a_fuzz (name5);
    check_repeat_a_fuzz (name6_parser <char>());
}

BOOST_AUTO_TEST_SUITE_END()

