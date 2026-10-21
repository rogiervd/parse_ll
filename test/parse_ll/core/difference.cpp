// SPDX-FileCopyrightText: Copyright 2012 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

/** \file
Test difference parser.
*/

#define BOOST_TEST_MODULE difference_parser
#include <boost/test/unit_test.hpp>

#include "parse_ll/core/difference.hpp"

#include <string>

#include "range/core.hpp"
#include "range/std/string.hpp"

#include "parse_ll/core/char.hpp"
#include "parse_ll/core/literal.hpp"
#include "parse_ll/core/repeat.hpp"
#include "parse_ll/core/transform.hpp"

#include <boost/phoenix/core/value.hpp>
#include "../helper/fuzz_parser.hpp"

BOOST_AUTO_TEST_SUITE(test_parse_difference)


BOOST_AUTO_TEST_CASE(test_difference)
{
    using range::drop;
    using range::empty;
    using range::first;

    using parse_ll::output;
    using parse_ll::parse;
    using parse_ll::rest;
    using parse_ll::success;

    using boost::phoenix::val;
    {
        std::string r("");
        {
            auto parser = parse_ll::char_('a') - parse_ll::char_('b');
            auto result = parse(parser, r);
            BOOST_CHECK(!success(result));
        }
    }
    {
        std::string r("ab");
        {
            auto parser = fuzz(
                fuzz(parse_ll::char_('a')[val(1)])
                - fuzz(parse_ll::char_('b')));
            auto result1 = parse(parser, r);
            BOOST_CHECK(success(result1));
            BOOST_CHECK_EQUAL(output(result1), 1);
            auto result2 = parse(parser, rest(result1));
            BOOST_CHECK(!success(result2));
        }
    }
    {
        std::string r("45 a");
        {
            auto parser = fuzz(*fuzz(parse_ll::char_ - parse_ll::literal(' ')));
            auto result = parse(parser, r);
            BOOST_CHECK(success(result));
            BOOST_CHECK_EQUAL(first(output(result)), '4');
            BOOST_CHECK_EQUAL(first(drop(output(result))), '5');
            BOOST_CHECK(::range::empty(drop(drop(output(result)))));
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()
