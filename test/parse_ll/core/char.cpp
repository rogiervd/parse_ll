// SPDX-FileCopyrightText: Copyright 2012 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

/** \file
Test char_parser.
*/

#define BOOST_TEST_MODULE char_parser
#include <boost/test/unit_test.hpp>

#include "parse_ll/core/char.hpp"

#include <type_traits>

#include <string>

#include "range/core.hpp"
#include "range/std/string.hpp"

#include "../helper/fuzz_parser.hpp"

BOOST_AUTO_TEST_SUITE(test_parse_char_parser)


BOOST_AUTO_TEST_CASE(test_char)
{
    using range::drop;
    using range::empty;
    using range::first;

    using parse_ll::output;
    using parse_ll::parse;
    using parse_ll::rest;
    using parse_ll::success;
    {
        std::string r("");
        {
            auto parser = parse_ll::char_;
            auto result = parse(parser, r);
            BOOST_CHECK(!success(result));
        }
        {
            auto parser = parse_ll::char_('1');
            auto result = parse(parser, r);
            BOOST_CHECK(!success(result));
        }
    }
    {
        std::string r("ab");
        {
            auto parser = fuzz(parse_ll::char_);
            auto result = parse(parser, r);
            BOOST_CHECK(success(result));
            BOOST_CHECK_EQUAL(output(result), 'a');
            BOOST_CHECK_EQUAL(first(rest(result)), 'b');
        }
        {
            auto parser = parse_ll::char_('b');
            auto result = parse(parser, r);
            BOOST_CHECK(!success(result));
        }
        {
            auto parser = fuzz(parse_ll::char_('a'));
            auto result = parse(parser, r);
            BOOST_CHECK(success(result));
            BOOST_CHECK_EQUAL(output(result), 'a');
            BOOST_CHECK_EQUAL(first(rest(result)), 'b');
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()
