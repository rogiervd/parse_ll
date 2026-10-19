// SPDX-FileCopyrightText: Copyright 2012 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

/** \file
Test digit parser.
*/

#define BOOST_TEST_MODULE digit_parser
#include <boost/test/unit_test.hpp>

#include "parse_ll/number/digit.hpp"

#include <string>

#include "range/core.hpp"
#include "range/std/string.hpp"

#include "../helper/fuzz_parser.hpp"

BOOST_AUTO_TEST_SUITE(test_parse_digit)

BOOST_AUTO_TEST_CASE(test_digit)
{
    using range::empty;

    using parse_ll::output;
    using parse_ll::parse;
    using parse_ll::rest;
    using parse_ll::success;
    {
        std::string r("");
        {
            auto parser = parse_ll::digit;
            auto result = parse(parser, r);
            BOOST_CHECK(!success(result));
        }
    }
    {
        std::string r("a");
        {
            auto parser = fuzz(parse_ll::digit);
            auto result = parse(parser, r);
            BOOST_CHECK(!success(result));
        }
    }
    {
        std::string r("017");
        {
            auto parser = parse_ll::digit;
            auto result1 = parse(parser, r);
            BOOST_CHECK(success(result1));
            BOOST_CHECK_EQUAL(output(result1), 0);
            BOOST_CHECK(!empty(rest(result1)));

            auto result2 = parse(parser, rest(result1));
            BOOST_CHECK(success(result2));
            BOOST_CHECK_EQUAL(output(result2), 1);
            BOOST_CHECK(!empty(rest(result2)));

            auto result3 = parse(parser, rest(result2));
            BOOST_CHECK(success(result3));
            BOOST_CHECK_EQUAL(output(result3), 7);
            BOOST_CHECK(empty(rest(result3)));

            auto result4 = parse(parser, rest(result3));
            BOOST_CHECK(!success(result4));
        }
    }
    {
        std::string r("9");
        {
            auto parser = fuzz(parse_ll::digit);
            {
                auto result = parse(parser, r);
                BOOST_CHECK(success(result));
                BOOST_CHECK_EQUAL(output(result), 9);
                BOOST_CHECK(empty(rest(result)));
            }
            {
                auto o = output(parse(parser, r));
                BOOST_CHECK_EQUAL(o, 9);
            }
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()
