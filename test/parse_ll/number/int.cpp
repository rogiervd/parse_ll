/*
Copyright 2012 Rogier van Dalen.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

/** \file
Test int parser.
*/

#define BOOST_TEST_MODULE int_parser
#include "utility/test/boost_unit_test.hpp"

#include "parse_ll/number/int.hpp"

#include <string>

#include "range/core.hpp"
#include "range/std/container.hpp"

#include <boost/math/special_functions/sign.hpp>

#include "../helper/fuzz_parser.hpp"
#include "parse_ll/core/skip.hpp"
#include "parse_ll/core/literal.hpp"

BOOST_AUTO_TEST_SUITE(test_parse_int)

BOOST_AUTO_TEST_CASE (test_int) {
    using range::empty; using range::first; using range::drop;

    using parse_ll::parse;
    using parse_ll::success;
    using parse_ll::output;
    using parse_ll::rest;

    {
        std::string r ("");
        {
            auto parser = parse_ll::int_;
            auto result = parse (parser, r);
            BOOST_CHECK (!success (result));
        }
    }
    {
        std::string r ("a");
        {
            auto parser = parse_ll::int_;
            auto result = parse (parser, r);
            BOOST_CHECK (!success (result));
        }
    }
    {
        std::string r ("0");
        {
            auto parser = fuzz (parse_ll::int_);
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            BOOST_CHECK_EQUAL (output (result), 0);
            BOOST_CHECK (empty (rest (result)));
        }
    }
    {
        std::string r ("-0");
        {
            auto parser = parse_ll::skip (parse_ll::literal (' ')) [
                parse_ll::int_as <float>()];
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            BOOST_CHECK_EQUAL (output (result), -0);
            BOOST_CHECK (boost::math::signbit (output (result)));
            BOOST_CHECK (empty (rest (result)));
        }
    }
    {
        // Does not parse.
        std::string r ("- 0");
        {
            auto parser = parse_ll::skip (parse_ll::literal (' ')) [
                parse_ll::int_as <float>()];
            auto result = parse (parser, r);
            BOOST_CHECK (!success (result));
        }
    }
    {
        std::string r ("+6");
        {
            auto parser = fuzz (parse_ll::int_);
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            BOOST_CHECK_EQUAL (output (result), 6);
            BOOST_CHECK (empty (rest (result)));
        }
    }
    {
        std::string r ("-6371 45");
        {
            // Only the first int should be parsed.
            auto parser = parse_ll::skip (parse_ll::literal (' ')) [
                parse_ll::int_];
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            BOOST_CHECK_EQUAL (output (result), -6371);
            BOOST_CHECK_EQUAL (first (rest (result)), ' ');
        }
    }
    {
        std::string r ("63710.");
        {
            auto parser = fuzz (parse_ll::int_as <int>());
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            BOOST_CHECK_EQUAL (output (result), 63710);
            BOOST_CHECK_EQUAL (first (rest (result)), '.');
            BOOST_CHECK (empty (drop (rest (result))));
        }
    }
    {
        // This fits in a 32-bit unsigned, but not in a 32-bit signed integer!
        std::string r ("4000000000");
        {
            auto parser = parse_ll::int_;
            // It is not defined exactly when the error is thrown, so
            // call .parse().output() at once.
            BOOST_CHECK_THROW (output (parse (parser, r)), std::overflow_error);
        }
    }
    {
        std::string r ("6123789457890123710.");
        {
            auto parser = fuzz (parse_ll::int_as <short>());
            BOOST_CHECK_THROW (output (parse (parser, r)), std::overflow_error);
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()
