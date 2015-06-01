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
Test unsigned parser.
*/

#define BOOST_TEST_MODULE unsigned_parser
#include "utility/test/boost_unit_test.hpp"

#include "parse_ll/number/unsigned.hpp"

#include <string>

#include "range/core.hpp"
#include "range/std/container.hpp"

#include "../helper/fuzz_parser.hpp"

#include "parse_ll/core/skip.hpp"
#include "parse_ll/core/literal.hpp"
#include "parse_ll/core/sequence.hpp"

BOOST_AUTO_TEST_SUITE(test_parse_unsigned)

BOOST_AUTO_TEST_CASE (test_unsigned) {
    using range::empty; using range::first; using range::drop;

    using parse_ll::parse;
    using parse_ll::success;
    using parse_ll::output;
    using parse_ll::rest;

    {
        std::string r ("");
        {
            auto parser = parse_ll::unsigned_;
            auto result = parse (parser, r);
            BOOST_CHECK (!success (result));
        }
    }
    {
        std::string r ("a");
        {
            auto parser = parse_ll::unsigned_;
            auto result = parse (parser, r);
            BOOST_CHECK (!success (result));
        }
    }
    {
        std::string r ("0");
        {
            auto parser = parse_ll::unsigned_;
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            BOOST_CHECK_EQUAL (output (result), 0);
            BOOST_CHECK (empty (rest (result)));
        }
    }
    {
        std::string r ("6");
        {
            auto parser = fuzz (parse_ll::unsigned_);
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            BOOST_CHECK_EQUAL (output (result), 6);
            BOOST_CHECK (empty (rest (result)));
        }
    }
    {
        std::string r ("6371 45 ");
        {
            // The skip parser should not be used inside the unsigned!
            auto parser = parse_ll::skip (parse_ll::literal (' ')) [
                parse_ll::unsigned_];
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            BOOST_CHECK_EQUAL (output (result), 6371);
            BOOST_CHECK_EQUAL (first (rest (result)), ' ');
        }
        {
            auto parser = parse_ll::skip (parse_ll::literal (' ')) [
                parse_ll::unsigned_ >> parse_ll::unsigned_];
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            BOOST_CHECK_EQUAL (std::get <0> (output (result)), 6371);
            BOOST_CHECK_EQUAL (std::get <1> (output (result)), 45);
            BOOST_CHECK_EQUAL (first (rest (result)), ' ');
        }
    }
    {
        std::string r ("63710.");
        {
            auto parser = fuzz (parse_ll::unsigned_as <int>());
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            BOOST_CHECK_EQUAL (output (result), 63710);
            BOOST_CHECK_EQUAL (first (rest (result)), '.');
            BOOST_CHECK (empty (drop (rest (result))));
        }
    }
    {
        std::string r ("6123789457890123710.");
        {
            auto parser = parse_ll::unsigned_as <unsigned short>();
            // It is not defined when the error is thrown exactly, so
            // call .parse().output() at once.
            BOOST_CHECK_THROW (output (parse (parser, r)), std::overflow_error);
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()
