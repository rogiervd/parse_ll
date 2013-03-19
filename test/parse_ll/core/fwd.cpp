/*
Copyright 2013 Rogier van Dalen.

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
Test the file fwd.hpp.
*/

// Make sure it contains all the includes it needs.
#include "parse_ll/core/fwd.hpp"

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE fwd_hpp
#include <boost/test/unit_test.hpp>

struct invalid : parse_ll::operation::unimplemented {};
struct valid {};

BOOST_AUTO_TEST_SUITE(test_fwd_hpp)

BOOST_AUTO_TEST_CASE (test_unimplemented) {
    static_assert (!parse_ll::operation::is_implemented <invalid>::value, "");
    static_assert (parse_ll::operation::is_implemented <valid>::value, "");
}

BOOST_AUTO_TEST_SUITE_END()

