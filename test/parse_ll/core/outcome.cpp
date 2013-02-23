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
Test core/outcome.hpp.
*/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE outcome
#include <boost/test/unit_test.hpp>

#include "parse_ll/core/outcome.hpp"

BOOST_AUTO_TEST_SUITE(test_outcome)

struct A {
    int i;
    A (int i) : i (i) {}
    A() : i (-1) {}
};

struct B {
    int i;
    B (A a) : i (a.i) {}
};

struct C {
    int i;
    C (B b) : i (b.i) {}
};

struct Range {
    int j;
    explicit Range (int j) : j (j) {}
};

using parse_ll::simple_outcome;
using parse_ll::successful;
using parse_ll::failed;

using parse_ll::success;
using parse_ll::output;
using parse_ll::rest;

BOOST_AUTO_TEST_CASE (test_outcome) {
    {
        A a (3);
        Range r (78);

        /**** failed ****/
        failed failed_a;
        BOOST_CHECK (!success (failed_a));
        // output (failed_a) and rest (failed_a) yield compile-time errors.

        // Just being facetious for consistency.
        failed failed_void = failed_a;
        BOOST_CHECK (!success (failed_void));

        /**** successful ****/
        successful <A, Range> successful_a (a, r);
        BOOST_CHECK (success (successful_a));
        BOOST_CHECK_EQUAL (output (successful_a).i, 3);
        BOOST_CHECK_EQUAL (rest (successful_a).j, 78);
        {
            // Plain copy
            successful <A, Range> successful_a_2 = successful_a;
            BOOST_CHECK (success (successful_a_2));
            BOOST_CHECK_EQUAL (output (successful_a_2).i, 3);
            BOOST_CHECK_EQUAL (rest (successful_a_2).j, 78);
        }
        {
            // Conversion from A to B.
            successful <B, Range> successful_b = successful_a;
            BOOST_CHECK (success (successful_b));
            BOOST_CHECK_EQUAL (output (successful_b).i, 3);
            BOOST_CHECK_EQUAL (rest (successful_b).j, 78);
        }
        {
            // Two-step conversion from A to C.
            successful <C, Range> successful_c = successful_a;
            BOOST_CHECK (success (successful_c));
            BOOST_CHECK_EQUAL (output (successful_c).i, 3);
            BOOST_CHECK_EQUAL (rest (successful_c).j, 78);
        }

        // successful with default-constructed a.
        successful <A, Range> successful_a_2 (r);
        BOOST_CHECK (success (successful_a_2));
        BOOST_CHECK_EQUAL (output (successful_a_2).i, -1);
        BOOST_CHECK_EQUAL (rest (successful_a_2).j, 78);

        // successful with void.
        successful <void, Range> successful_void (r);
        BOOST_CHECK (success (successful_void));
        static_assert (std::is_same <decltype (output (successful_void)), void
            >::value, "");
        BOOST_CHECK_EQUAL (rest (successful_void).j, 78);

        /**** Copy into simple_outcome ****/
        {
            simple_outcome <A, Range> failed_a_2 = failed_a;
            BOOST_CHECK (!success (failed_a_2));
        }
        {
            simple_outcome <void, Range> failed_void_2 = failed_void;
            BOOST_CHECK (!success (failed_void_2));
        }
        {
            simple_outcome <A, Range> successful_a_3 = successful_a;
            BOOST_CHECK (success (successful_a_3));
            BOOST_CHECK_EQUAL (output (successful_a_3).i, 3);
            BOOST_CHECK_EQUAL (rest (successful_a_3).j, 78);
        }
        {
            simple_outcome <void, Range> successful_void_2 = successful_void;
            BOOST_CHECK (success (successful_void_2));
            BOOST_CHECK_EQUAL (rest (successful_void_2).j, 78);
        }
    }

    {
        /**** simple_outcome ****/
        A a (4);
        Range r (89);

        simple_outcome <A, Range> failed_a =
            simple_outcome <A, Range> (failed());
        BOOST_CHECK (!success (failed_a));

        simple_outcome <void, Range> failed_void =
            simple_outcome <void, Range> (failed());
        BOOST_CHECK (!success (failed_void));

        simple_outcome <A, Range> successful_a =
            simple_outcome <A, Range> (a, r);
        BOOST_CHECK (success (successful_a));
        BOOST_CHECK_EQUAL (output (successful_a).i, 4);
        BOOST_CHECK_EQUAL (rest (successful_a).j, 89);

        // successful with default-constructed a.
        simple_outcome <A, Range> successful_a_2 (r);
        BOOST_CHECK (success (successful_a_2));
        BOOST_CHECK_EQUAL (output (successful_a_2).i, -1);
        BOOST_CHECK_EQUAL (rest (successful_a_2).j, 89);

        {
            // Conversion from A to B.
            simple_outcome <B, Range> successful_b = successful_a;
            BOOST_CHECK (success (successful_b));
            BOOST_CHECK_EQUAL (output (successful_b).i, 4);
            BOOST_CHECK_EQUAL (rest (successful_b).j, 89);
        }
        {
            // Two-step conversion from A to C.
            simple_outcome <C, Range> successful_c = successful_a;
            BOOST_CHECK (success (successful_c));
            BOOST_CHECK_EQUAL (output (successful_c).i, 4);
            BOOST_CHECK_EQUAL (rest (successful_c).j, 89);
        }

        simple_outcome <void, Range> successful_void =
            simple_outcome <void, Range> (r);
        BOOST_CHECK (success (successful_void));
        static_assert (std::is_same <decltype (output (successful_void)), void
            >::value, "");
        BOOST_CHECK_EQUAL (rest (successful_void).j, 89);

        /**** Copy into failed ****/
        {
            failed failed_a_2 = failed_a;
            BOOST_CHECK (!success (failed_a_2));
        }
        {
            failed failed_void_2 = failed_void;
            BOOST_CHECK (!success (failed_void_2));
        }

        /**** Copy into successful ****/
        {
            successful <A, Range> successful_a_2 = successful_a;
            BOOST_CHECK (success (successful_a_2));
            BOOST_CHECK_EQUAL (output (successful_a_2).i, 4);
            BOOST_CHECK_EQUAL (rest (successful_a_2).j, 89);
        }
        {
            successful <void, Range> successful_void_2 = successful_void;
            BOOST_CHECK (success (successful_void_2));
            BOOST_CHECK_EQUAL (rest (successful_void_2).j, 89);
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()

