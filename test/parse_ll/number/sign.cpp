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
Test sign parser.
*/

#define BOOST_TEST_MODULE sign_parser
#include "utility/test/boost_unit_test.hpp"

#include "parse_ll/number/sign.hpp"

#include <string>

#include "range/core.hpp"
#include "range/std/container.hpp"

#include "../helper/fuzz_parser.hpp"

BOOST_AUTO_TEST_SUITE(test_parse_sign)

BOOST_AUTO_TEST_CASE (test_sign) {
    using range::empty; using range::first; using range::drop;

    using parse_ll::parse;
    using parse_ll::success;
    using parse_ll::output;
    using parse_ll::rest;
    {
        std::string r ("");
        {
            auto parser = parse_ll::sign;
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            BOOST_CHECK_EQUAL (output (result), +1);
        }
    }
    {
        std::string r ("0");
        {
            auto parser = parse_ll::sign;
            auto result = parse (parser, r);
            BOOST_CHECK_EQUAL (output (result), +1);
            BOOST_CHECK_EQUAL (first (rest (result)), '0');
        }
    }
    {
        std::string r ("+0");
        {
            auto parser = fuzz (parse_ll::sign);
            {
                auto result = parse (parser, r);
                BOOST_CHECK_EQUAL (output (result), +1);
                BOOST_CHECK_EQUAL (first (rest (result)), '0');
            }
            {
                auto o = output (parse (parser, r));
                BOOST_CHECK_EQUAL (o, +1);
            }
        }
    }
    {
        std::string r ("-0");
        auto parser = fuzz (parse_ll::sign);
        {
            auto result = parse (parser, r);
            BOOST_CHECK_EQUAL (output (result), -1);
            BOOST_CHECK_EQUAL (first (rest (result)), '0');
        }
        {
            auto o = output (parse (parser, r));
            BOOST_CHECK_EQUAL (o, -1);
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()
