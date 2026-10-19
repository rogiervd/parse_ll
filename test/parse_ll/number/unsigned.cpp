// SPDX-FileCopyrightText: Copyright 2012 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

/** \file
Test unsigned parser.
*/

#define BOOST_TEST_MODULE unsigned_parser
#include <boost/test/unit_test.hpp>

#include "parse_ll/number/unsigned.hpp"

#include <string>

#include "range/core.hpp"
#include "range/std/string.hpp"

#include "../helper/fuzz_parser.hpp"

#include "parse_ll/core/literal.hpp"
#include "parse_ll/core/sequence.hpp"
#include "parse_ll/core/skip.hpp"

BOOST_AUTO_TEST_SUITE(test_parse_unsigned)

BOOST_AUTO_TEST_CASE(test_unsigned)
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
            auto parser = parse_ll::unsigned_;
            auto result = parse(parser, r);
            BOOST_CHECK(!success(result));
        }
    }
    {
        std::string r("a");
        {
            auto parser = parse_ll::unsigned_;
            auto result = parse(parser, r);
            BOOST_CHECK(!success(result));
        }
    }
    {
        std::string r("0");
        {
            auto parser = parse_ll::unsigned_;
            auto result = parse(parser, r);
            BOOST_CHECK(success(result));
            BOOST_CHECK_EQUAL(output(result), 0u);
            BOOST_CHECK(empty(rest(result)));
        }
    }
    {
        std::string r("6");
        {
            auto parser = fuzz(parse_ll::unsigned_);
            auto result = parse(parser, r);
            BOOST_CHECK(success(result));
            BOOST_CHECK_EQUAL(output(result), 6u);
            BOOST_CHECK(empty(rest(result)));
        }
    }
    {
        std::string r("6371 45 ");
        {
            // The skip parser should not be used inside the unsigned!
            auto parser =
                parse_ll::skip(parse_ll::literal(' '))[parse_ll::unsigned_];
            auto result = parse(parser, r);
            BOOST_CHECK(success(result));
            BOOST_CHECK_EQUAL(output(result), 6371u);
            BOOST_CHECK_EQUAL(first(rest(result)), ' ');
        }
        {
            auto parser = parse_ll::skip(
                parse_ll::literal(
                    ' '))[parse_ll::unsigned_ >> parse_ll::unsigned_];
            auto result = parse(parser, r);
            BOOST_CHECK(success(result));
            BOOST_CHECK_EQUAL(std::get<0>(output(result)), 6371u);
            BOOST_CHECK_EQUAL(std::get<1>(output(result)), 45u);
            BOOST_CHECK_EQUAL(first(rest(result)), ' ');
        }
    }
    {
        std::string r("63710.");
        {
            auto parser = fuzz(parse_ll::unsigned_as<int>());
            auto result = parse(parser, r);
            BOOST_CHECK(success(result));
            BOOST_CHECK_EQUAL(output(result), 63710);
            BOOST_CHECK_EQUAL(first(rest(result)), '.');
            BOOST_CHECK(empty(drop(rest(result))));
        }
    }
    {
        std::string r("6123789457890123710.");
        {
            auto parser = parse_ll::unsigned_as<unsigned short>();
            // It is not defined when the error is thrown exactly, so
            // call .parse().output() at once.
            BOOST_CHECK_THROW(output(parse(parser, r)), std::overflow_error);
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()
