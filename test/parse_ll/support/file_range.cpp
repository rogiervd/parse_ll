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
Test file_range.
*/

#define BOOST_TEST_MODULE file_range
#include "utility/test/boost_unit_test.hpp"

#include "parse_ll/support/file_range.hpp"

#include <type_traits>
#include <stdexcept>

BOOST_AUTO_TEST_SUITE(test_parse_file_range)

BOOST_AUTO_TEST_CASE (test_file_range) {
    assert (boost::unit_test::framework::master_test_suite().argc == 2);
    std::string example_file_name =
        boost::unit_test::framework::master_test_suite().argv [1];

    using range::empty;
    using range::first;
    using range::drop;
    {
        BOOST_CHECK_THROW (range::file_range r ("non_existing_file.txt"),
            std::exception);
    }
    {
        range::file_range r (example_file_name);
        BOOST_CHECK_EQUAL (first (r), 'T');
        r = drop (r);
        BOOST_CHECK_EQUAL (first (r), 'e');
        r = drop (r);
        BOOST_CHECK_EQUAL (first (r), 's');
        r = drop (r);
        BOOST_CHECK_EQUAL (first (r), 't');
        r = drop (r);
        BOOST_CHECK_EQUAL (first (r), '.');
        r = drop (r);
        BOOST_CHECK_EQUAL (first (r), '\n');
        r = drop (r);
        auto save_r = r;
        BOOST_CHECK_EQUAL (first (r), 'E');
        r = drop (r);
        BOOST_CHECK_EQUAL (first (r), 'n');
        r = drop (r);
        BOOST_CHECK_EQUAL (first (r), 'd');
        r = drop (r);
        BOOST_CHECK_EQUAL (first (r), '\n');
        r = drop (r);
        BOOST_CHECK (empty (r));

        // Second pass
        BOOST_CHECK_EQUAL (first (save_r), 'E');
        save_r = drop (save_r);
        BOOST_CHECK_EQUAL (first (save_r), 'n');
        save_r = drop (save_r);
        BOOST_CHECK_EQUAL (first (save_r), 'd');
        save_r = drop (save_r);
        BOOST_CHECK_EQUAL (first (save_r), '\n');
        save_r = drop (save_r);
        BOOST_CHECK (empty (save_r));
    }
}

BOOST_AUTO_TEST_SUITE_END()
