// SPDX-FileCopyrightText: Copyright 2012 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

/** \file
Test sign parser.
*/

#define BOOST_TEST_MODULE sign_parser
#include <boost/test/unit_test.hpp>

#include "parse_ll/number/sign.hpp"

#include <string>

#include "range/core.hpp"
#include "range/std/string.hpp"

#include "../helper/fuzz_parser.hpp"

BOOST_AUTO_TEST_SUITE(test_parse_sign)

BOOST_AUTO_TEST_CASE(test_sign)
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
            auto parser = parse_ll::sign;
            auto result = parse(parser, r);
            BOOST_CHECK(success(result));
            BOOST_CHECK_EQUAL(output(result), +1);
        }
    }
    {
        std::string r("0");
        {
            auto parser = parse_ll::sign;
            auto result = parse(parser, r);
            BOOST_CHECK_EQUAL(output(result), +1);
            BOOST_CHECK_EQUAL(first(rest(result)), '0');
        }
    }
    {
        std::string r("+0");
        {
            auto parser = fuzz(parse_ll::sign);
            {
                auto result = parse(parser, r);
                BOOST_CHECK_EQUAL(output(result), +1);
                BOOST_CHECK_EQUAL(first(rest(result)), '0');
            }
            {
                auto o = output(parse(parser, r));
                BOOST_CHECK_EQUAL(o, +1);
            }
        }
    }
    {
        std::string r("-0");
        auto parser = fuzz(parse_ll::sign);
        {
            auto result = parse(parser, r);
            BOOST_CHECK_EQUAL(output(result), -1);
            BOOST_CHECK_EQUAL(first(rest(result)), '0');
        }
        {
            auto o = output(parse(parser, r));
            BOOST_CHECK_EQUAL(o, -1);
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()
