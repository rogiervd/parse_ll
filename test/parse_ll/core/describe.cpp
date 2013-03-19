/*
Copyright 2012 Rogier van Dalen.

This file is part of Rogier van Dalen's LL Parser library for C++.

This library is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/** \file
Test describe.
*/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE describer
#include <boost/test/unit_test.hpp>

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
