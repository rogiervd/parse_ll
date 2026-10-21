// SPDX-FileCopyrightText: Copyright 2012 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

/** \file
Test describe.
*/

#define BOOST_TEST_MODULE describer
#include <boost/test/unit_test.hpp>

#include "parse_ll/core/describe.hpp"

#include <string>
#include "../helper/fuzz_parser.hpp"
#include "parse_ll/core/literal.hpp"

template <class SubParser> struct non_existent_parser
: public parse_ll::parser_base<non_existent_parser<SubParser>> {};

struct non_existent_parser_tag;
namespace parse_ll {
template <class SubParser>
struct decayed_parser_tag<non_existent_parser<SubParser>>
{
    typedef non_existent_parser_tag type;
};
}  // namespace parse_ll

BOOST_AUTO_TEST_SUITE(test_describe)

BOOST_AUTO_TEST_CASE(test_describe)
{
    auto l = parse_ll::literal("hello");
    auto parser = non_existent_parser<decltype(l)>();
    std::string description = parse_ll::describe(parser);
    BOOST_CHECK(description.find("non_existent_parser") != std::string::npos);
}

BOOST_AUTO_TEST_SUITE_END()
