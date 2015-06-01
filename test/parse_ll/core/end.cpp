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
Test end_parser.
*/

#define BOOST_TEST_MODULE end_parser
#include "utility/test/boost_unit_test.hpp"

#include "parse_ll/core/end.hpp"

#include <string>

#include "range/core.hpp"
#include "range/std/container.hpp"

#include <type_traits>

#include "../helper/fuzz_parser.hpp"

BOOST_AUTO_TEST_SUITE(test_parse_end)

BOOST_AUTO_TEST_CASE (test_end) {
    using range::empty;

    using parse_ll::parse;
    using parse_ll::success;
    using parse_ll::output;
    using parse_ll::rest;

    {
        std::string r ("");
        {
            auto parser = parse_ll::end;
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            static_assert (std::is_same <decltype (output (result)), void
                >::value, "Output should be void");
            BOOST_CHECK (empty (rest (result)));
        }
        {
            auto parser = fuzz (parse_ll::end);
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            BOOST_CHECK (empty (rest (result)));
        }
    }
    {
        std::string r ("a");
        {
            auto parser = fuzz (parse_ll::end);
            auto result = parse (parser, r);
            BOOST_CHECK (!success (result));
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

