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
Test describe.
*/

#define BOOST_TEST_MODULE describer
#include "utility/test/boost_unit_test.hpp"

#include "parse_ll/core/describe.hpp"

#include <string>
#include "parse_ll/core/literal.hpp"
#include "../helper/fuzz_parser.hpp"

template <class SubParser> struct non_existent_parser
: public parse_ll::parser_base <non_existent_parser <SubParser> > {};

struct non_existent_parser_tag;
namespace parse_ll {
    template <class SubParser>
        struct decayed_parser_tag <non_existent_parser <SubParser>>
    { typedef non_existent_parser_tag type; };
} // namespace parse_ll

BOOST_AUTO_TEST_SUITE(test_describe)

BOOST_AUTO_TEST_CASE (test_describe) {
    auto l = parse_ll::literal ("hello");
    auto parser = non_existent_parser <decltype (l)>();
    std::string description = parse_ll::describe (parser);
    BOOST_CHECK (description.find ("non_existent_parser") != std::string::npos);
}

BOOST_AUTO_TEST_SUITE_END()
