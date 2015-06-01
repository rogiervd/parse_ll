/*
Copyright 2013 Rogier van Dalen.

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
Test the file fwd.hpp.
*/

// Make sure it contains all the includes it needs.
#include "parse_ll/core/fwd.hpp"

#define BOOST_TEST_MODULE fwd_hpp
#include "utility/test/boost_unit_test.hpp"

struct invalid : parse_ll::operation::unimplemented {};
struct valid {};

BOOST_AUTO_TEST_SUITE(test_fwd_hpp)

BOOST_AUTO_TEST_CASE (test_unimplemented) {
    static_assert (!parse_ll::operation::is_implemented <invalid>::value, "");
    static_assert (parse_ll::operation::is_implemented <valid>::value, "");
}

BOOST_AUTO_TEST_SUITE_END()

