// SPDX-FileCopyrightText: Copyright 2013 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

/** \file
Test the file fwd.hpp.
*/

// Make sure it contains all the includes it needs.
#include "parse_ll/core/fwd.hpp"

#define BOOST_TEST_MODULE fwd_hpp
#include <boost/test/unit_test.hpp>

struct invalid : parse_ll::operation::unimplemented {};
struct valid {};

BOOST_AUTO_TEST_SUITE(test_fwd_hpp)

BOOST_AUTO_TEST_CASE(test_unimplemented)
{
    static_assert(!parse_ll::operation::is_implemented<invalid>::value, "");
    static_assert(parse_ll::operation::is_implemented<valid>::value, "");
}

BOOST_AUTO_TEST_SUITE_END()
