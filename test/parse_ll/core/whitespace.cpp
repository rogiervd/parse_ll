// SPDX-FileCopyrightText: Copyright 2012 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

/** \file
Test whitespace parsers.
*/

#define BOOST_TEST_MODULE whitespace_parser
#include <boost/test/unit_test.hpp>

#include "parse_ll/core/whitespace.hpp"

#include <type_traits>

#include <string>

#include "range/core.hpp"
#include "range/std/string.hpp"

#include "../helper/fuzz_parser.hpp"

BOOST_AUTO_TEST_SUITE(test_parse_whitespace_parser)

BOOST_AUTO_TEST_CASE(test_whitespace)
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
            auto parser = parse_ll::space;
            auto result = parse(parser, r);
            BOOST_CHECK(!success(result));
        }
        {
            auto parser = parse_ll::whitespace;
            auto result = parse(parser, r);
            BOOST_CHECK(success(result));
        }
        {
            auto parser = parse_ll::newline;
            auto result = parse(parser, r);
            BOOST_CHECK(!success(result));
        }
        {
            auto parser = parse_ll::one_horizontal_space;
            auto result = parse(parser, r);
            BOOST_CHECK(!success(result));
        }
        {
            auto parser = parse_ll::horizontal_space;
            auto result = parse(parser, r);
            BOOST_CHECK(success(result));
        }
        {
            auto parser = fuzz(parse_ll::vertical_space);
            auto result = parse(parser, r);
            BOOST_CHECK(success(result));
        }
        {
            auto parser = parse_ll::one_whitespace;
            auto result = parse(parser, r);
            BOOST_CHECK(!success(result));
        }
        {
            auto parser = parse_ll::whitespace;
            auto result = parse(parser, r);
            BOOST_CHECK(success(result));
        }
    }
    {
        std::string r(" b");
        {
            auto parser = parse_ll::space;
            auto result = parse(parser, r);
            BOOST_CHECK(success(result));
            static_assert(
                std::is_same<decltype(output(result)), void>::value,
                "Parser should have no output type");
            BOOST_CHECK_EQUAL(first(rest(result)), 'b');
        }
        {
            auto parser = fuzz(parse_ll::one_horizontal_space);
            auto result = parse(parser, r);
            BOOST_CHECK(success(result));
            static_assert(
                std::is_same<decltype(output(result)), void>::value,
                "Parser should have no output type");
            BOOST_CHECK_EQUAL(first(rest(result)), 'b');
        }
        {
            auto parser = fuzz(parse_ll::horizontal_space);
            auto result = parse(parser, r);
            BOOST_CHECK(success(result));
            static_assert(
                std::is_same<decltype(output(result)), void>::value,
                "Parser should have no output type");
            BOOST_CHECK_EQUAL(first(rest(result)), 'b');
        }
        {
            auto parser = parse_ll::vertical_space;
            auto result = parse(parser, r);
            BOOST_CHECK(success(result));
            static_assert(
                std::is_same<decltype(output(result)), void>::value,
                "Parser should have no output type");
            BOOST_CHECK_EQUAL(first(rest(result)), ' ');
        }
        {
            auto parser = parse_ll::one_whitespace;
            auto result = parse(parser, r);
            BOOST_CHECK(success(result));
            BOOST_CHECK_EQUAL(first(rest(result)), 'b');
        }
        {
            auto parser = parse_ll::whitespace;
            auto result = parse(parser, r);
            BOOST_CHECK(success(result));
            BOOST_CHECK_EQUAL(first(rest(result)), 'b');
        }
    }
    {
        std::string r("\r\nc");
        {
            auto parser = parse_ll::carriage_return;
            auto result = parse(parser, r);
            BOOST_CHECK(success(result));
            BOOST_CHECK_EQUAL(first(rest(result)), '\n');
        }
        {
            auto parser = fuzz(parse_ll::newline);
            auto result = parse(parser, r);
            BOOST_CHECK(success(result));
            BOOST_CHECK_EQUAL(first(rest(result)), 'c');
        }
        {
            auto parser = parse_ll::one_horizontal_space;
            auto result = parse(parser, r);
            BOOST_CHECK(!success(result));
        }
        {
            auto parser = parse_ll::vertical_space;
            auto result = parse(parser, r);
            BOOST_CHECK(success(result));
            BOOST_CHECK_EQUAL(first(rest(result)), 'c');
        }
        {
            auto parser = fuzz(parse_ll::whitespace);
            auto result = parse(parser, r);
            BOOST_CHECK(success(result));
            BOOST_CHECK_EQUAL(first(rest(result)), 'c');
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()
