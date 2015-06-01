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
Test optional parser.
*/

#define BOOST_TEST_MODULE optional_parser
#include "utility/test/boost_unit_test.hpp"

#include "parse_ll/core/optional.hpp"

#include <string>

#include "range/core.hpp"
#include "range/std/container.hpp"

#include "parse_ll/core/char.hpp"
#include "parse_ll/core/nothing.hpp"
#include "../helper/fuzz_parser.hpp"

BOOST_AUTO_TEST_SUITE(test_parse_optional)


BOOST_AUTO_TEST_CASE (test_optional) {
    using range::empty; using range::first;

    using parse_ll::parse;
    using parse_ll::success;
    using parse_ll::output;
    using parse_ll::rest;
    {
        std::string r ("");
        {
            auto parser = -parse_ll::char_ ('a');
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            BOOST_CHECK (!output (result));
            BOOST_CHECK (empty (rest (result)));
        }
        {
            // Test with void output type.
            auto parser = fuzz (-parse_ll::nothing);
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            static_assert (
                std::is_same <decltype (parse_ll::output (result)), void
                    >::value, "Should output void");
            BOOST_CHECK (empty (rest (result)));
        }
    }
    {
        std::string r ("a");
        {
            auto parser = fuzz (-parse_ll::char_ ('b'));
            auto result = parse (parser, r);
            // Should be successful, but still not use up any input.
            BOOST_CHECK (success (result));
            BOOST_CHECK_EQUAL (first (rest (result)), 'a');
            BOOST_CHECK (!output (result));
        }
        {
            auto parser = fuzz (-parse_ll::char_ ('a'));
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            BOOST_CHECK_EQUAL (output (result).get(), 'a');
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()

