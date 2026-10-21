// SPDX-FileCopyrightText: Copyright 2012 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

/** \file
Test nothing_parser.
*/

#define BOOST_TEST_MODULE nothing_parser
#include <boost/test/unit_test.hpp>

#include "parse_ll/core/nothing.hpp"

#include <type_traits>

#include <string>

#include "range/core.hpp"
#include "range/std/string.hpp"

#include "../helper/fuzz_parser.hpp"

BOOST_AUTO_TEST_SUITE(test_parse_nothing)

BOOST_AUTO_TEST_CASE(test_nothing)
{
    using range::drop;
    using range::empty;
    using range::first;
    {
        std::string s("");
        {
            auto parser = parse_ll::nothing;
            auto result = parse_ll::parse(parser, s);
            BOOST_CHECK(parse_ll::success(result));
            BOOST_CHECK(empty(parse_ll::rest(result)));
        }
    }
    {
        std::string s("a");
        {
            auto parser = fuzz(parse_ll::nothing);
            auto result = parse_ll::parse(parser, s);
            // Should be successful, but still not use up any input.
            BOOST_CHECK(parse_ll::success(result));
            static_assert(
                std::is_same<decltype(parse_ll::output(result)), void>::value,
                "Should output void");
            BOOST_CHECK_EQUAL(first(parse_ll::rest(result)), 'a');
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()
