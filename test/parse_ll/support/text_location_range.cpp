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
Test text_location_range.
*/

#define BOOST_TEST_MODULE text_location_range
#include "utility/test/boost_unit_test.hpp"

#include "parse_ll/support/text_location_range.hpp"

#include <type_traits>
#include <stdexcept>
#include <iostream>
#include <string>

#include "range/core.hpp"
#include "range/std/container.hpp"

#include "parse_ll/support/file_range.hpp"

BOOST_AUTO_TEST_SUITE(test_parse_text_location_range)

BOOST_AUTO_TEST_CASE (test_text_location_range) {
    using range::empty;
    using range::first;
    using range::drop;

    assert (boost::unit_test::framework::master_test_suite().argc == 2);
    std::string example_file_name =
        boost::unit_test::framework::master_test_suite().argv [1];
    std::cout << "Using as an test file: " << example_file_name << std::endl;
    {
        range::text_location_range <range::file_range> r (
            example_file_name);

        BOOST_CHECK_EQUAL (r.line(),  0);
        BOOST_CHECK_EQUAL (r.column(),  0);
        BOOST_CHECK_EQUAL (first (r), '0');
        r = drop (r);
        BOOST_CHECK_EQUAL (first (r), '0');
        r = drop (r);

        r = drop (r);
        r = drop (r);
        r = drop (r);

        BOOST_CHECK_EQUAL (r.line(),  1);
        BOOST_CHECK_EQUAL (r.column(),  2);
        BOOST_CHECK_EQUAL (first (r), '1');
        r = drop (r);
        BOOST_CHECK_EQUAL (first (r), '2');
        r = drop (r);

        r = drop (r);
        r = drop (r);
        r = drop (r);
        r = drop (r);
        r = drop (r);

        BOOST_CHECK_EQUAL (r.line(),  4);
        BOOST_CHECK_EQUAL (r.column(),  0);
        BOOST_CHECK_EQUAL (first (r), '4');
        r = drop (r);
        BOOST_CHECK_EQUAL (first (r), '0');

        r = drop (r);
        r = drop (r);
        BOOST_CHECK_EQUAL (r.line(),  5);
        BOOST_CHECK_EQUAL (r.column(),  0);
        BOOST_CHECK_EQUAL (first (r), '5');
        r = drop (r);
        BOOST_CHECK_EQUAL (first (r), '0');
        r = drop (r);
        // Tab!
        r = drop (r);

        BOOST_CHECK_EQUAL (r.line(),  5);
        BOOST_CHECK_EQUAL (r.column(),  4);
        BOOST_CHECK_EQUAL (first (r), '5');
        r = drop (r);
        BOOST_CHECK_EQUAL (first (r), '4');
        r = drop (r);
    }
    {
        std::string s = "a\r\n b";
        auto core = range::view (s);
        range::text_location_range <decltype (core)> r (core);
        r = drop (r);
        // CR
        r = drop (r);
        BOOST_CHECK_EQUAL (r.line(),  1);
        BOOST_CHECK_EQUAL (r.column(),  0);
        // LF
        r = drop (r);
        BOOST_CHECK_EQUAL (r.line(),  1);
        BOOST_CHECK_EQUAL (r.column(),  0);
        r = drop (r);
        BOOST_CHECK_EQUAL (r.line(),  1);
        BOOST_CHECK_EQUAL (r.column(),  1);
        r = drop (r);
        BOOST_CHECK_EQUAL (r.line(),  1);
        BOOST_CHECK_EQUAL (r.column(),  2);
        BOOST_CHECK (empty (r));
    }
}

BOOST_AUTO_TEST_SUITE_END()

