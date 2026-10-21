// SPDX-FileCopyrightText: Copyright 2012 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

/** \file
Test end_parser.
*/

#define BOOST_TEST_MODULE end_parser
#include <boost/test/unit_test.hpp>

#include "parse_ll/core/end.hpp"

#include <string>

#include "range/core.hpp"
#include "range/std/string.hpp"

#include <type_traits>

#include "../helper/fuzz_parser.hpp"

BOOST_AUTO_TEST_SUITE(test_parse_end)

BOOST_AUTO_TEST_CASE(test_end)
{
    using range::empty;

    using parse_ll::output;
    using parse_ll::parse;
    using parse_ll::rest;
    using parse_ll::success;

    {
        std::string r("");
        {
            auto parser = parse_ll::end;
            auto result = parse(parser, r);
            BOOST_CHECK(success(result));
            static_assert(
                std::is_same<decltype(output(result)), void>::value,
                "Output should be void");
            BOOST_CHECK(empty(rest(result)));
        }
        {
            auto parser = fuzz(parse_ll::end);
            auto result = parse(parser, r);
            BOOST_CHECK(success(result));
            BOOST_CHECK(empty(rest(result)));
        }
    }
    {
        std::string r("a");
        {
            auto parser = fuzz(parse_ll::end);
            auto result = parse(parser, r);
            BOOST_CHECK(!success(result));
        }
        // This test should not require sequence.
        /*{
            auto parser = parse_ll::literal ('a') >> parse_ll::end;
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            BOOST_CHECK (empty (rest (result)));
        }*/
    }
}

BOOST_AUTO_TEST_SUITE_END()
