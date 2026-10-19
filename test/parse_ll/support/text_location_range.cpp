// SPDX-FileCopyrightText: Copyright 2012 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

/** \file
Test text_location_range.
*/

#define BOOST_TEST_MODULE text_location_range
#include <boost/test/unit_test.hpp>

#include "parse_ll/support/text_location_range.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <type_traits>

#include "range/core.hpp"
#include "range/std/string.hpp"

#include "parse_ll/support/file_range.hpp"

BOOST_AUTO_TEST_SUITE(test_parse_text_location_range)

BOOST_AUTO_TEST_CASE(test_text_location_range)
{
    using range::drop;
    using range::empty;
    using range::first;

    assert(boost::unit_test::framework::master_test_suite().argc == 2);
    std::string example_file_name =
        boost::unit_test::framework::master_test_suite().argv[1];
    std::cout << "Using as an test file: " << example_file_name << std::endl;
    {
        range::text_location_range<range::file_range> r(example_file_name);

        BOOST_CHECK_EQUAL(r.line(), 0u);
        BOOST_CHECK_EQUAL(r.column(), 0u);
        BOOST_CHECK_EQUAL(first(r), '0');
        r = drop(r);
        BOOST_CHECK_EQUAL(first(r), '0');
        r = drop(r);

        r = drop(r);
        r = drop(r);
        r = drop(r);

        BOOST_CHECK_EQUAL(r.line(), 1u);
        BOOST_CHECK_EQUAL(r.column(), 2u);
        BOOST_CHECK_EQUAL(first(r), '1');
        r = drop(r);
        BOOST_CHECK_EQUAL(first(r), '2');
        r = drop(r);

        r = drop(r);
        r = drop(r);
        r = drop(r);
        r = drop(r);
        r = drop(r);

        BOOST_CHECK_EQUAL(r.line(), 4u);
        BOOST_CHECK_EQUAL(r.column(), 0u);
        BOOST_CHECK_EQUAL(first(r), '4');
        r = drop(r);
        BOOST_CHECK_EQUAL(first(r), '0');

        r = drop(r);
        r = drop(r);
        BOOST_CHECK_EQUAL(r.line(), 5u);
        BOOST_CHECK_EQUAL(r.column(), 0u);
        BOOST_CHECK_EQUAL(first(r), '5');
        r = drop(r);
        BOOST_CHECK_EQUAL(first(r), '0');
        r = drop(r);
        // Tab!
        r = drop(r);

        BOOST_CHECK_EQUAL(r.line(), 5u);
        BOOST_CHECK_EQUAL(r.column(), 4u);
        BOOST_CHECK_EQUAL(first(r), '5');
        r = drop(r);
        BOOST_CHECK_EQUAL(first(r), '4');
        r = drop(r);
    }
    {
        std::string s = "a\r\n b";
        auto core = range::view(s);
        range::text_location_range<decltype(core)> r(core);
        r = drop(r);
        // CR
        r = drop(r);
        BOOST_CHECK_EQUAL(r.line(), 1u);
        BOOST_CHECK_EQUAL(r.column(), 0u);
        // LF
        r = drop(r);
        BOOST_CHECK_EQUAL(r.line(), 1u);
        BOOST_CHECK_EQUAL(r.column(), 0u);
        r = drop(r);
        BOOST_CHECK_EQUAL(r.line(), 1u);
        BOOST_CHECK_EQUAL(r.column(), 1u);
        r = drop(r);
        BOOST_CHECK_EQUAL(r.line(), 1u);
        BOOST_CHECK_EQUAL(r.column(), 2u);
        BOOST_CHECK(empty(r));
    }
}

BOOST_AUTO_TEST_SUITE_END()
