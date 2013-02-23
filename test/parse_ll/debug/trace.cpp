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
Test trace.
*/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE trace
#include <boost/test/unit_test.hpp>

#include "parse_ll/debug/trace.hpp"
#include "parse_ll/debug/ostream_observer.hpp"

#include <ostream>
#include <iostream>
#include <typeinfo>
#include <list>
#include <string>

#include <boost/any.hpp>

#include "range/core.hpp"
#include "range/std/container.hpp"

#include "parse_ll/core.hpp"
#include "parse_ll/support/text_location_range.hpp"

BOOST_AUTO_TEST_SUITE(print_trace)

using parse_ll::trace;
using parse_ll::ostream_observer;
using parse_ll::literal;
using parse_ll::parse;
using parse_ll::end;

BOOST_AUTO_TEST_CASE (test_trace) {
    PARSE_LL_DEFINE_NAMED_PARSER (string, (
        parse_ll::no_skip [+(parse_ll::char_ - parse_ll::one_whitespace)]));

    auto parser = trace (ostream_observer(),
        parse_ll::parse_policy::stop_at_no_skip()) [
            *literal ('a') >> string >> *(literal (' ') | literal ('b')) >> end
        ];

    std::string s = "aaastring b b";
    auto input1 = range::view (s);
    range::text_location_range <decltype (input1)> input (input1);

//    parse (parser, input);

//    BOOST_CHECK (false);
}

/// Simple observer that has very strict expectations.
struct expect_observer {
    template <class Parser> struct expect_start_parse {
        int depth;
        int line, column;
        expect_start_parse (int depth, int line, int column)
        : depth (depth), line (line), column (column) {}
    };
    template <class Parser> struct expect_successful_parse {
        int depth;
        int start_line, start_column, finish_line, finish_column;
        expect_successful_parse (int depth, int start_line, int start_column,
            int finish_line, int finish_column)
        : depth (depth), start_line (start_line), start_column (start_column),
            finish_line (finish_line), finish_column (finish_column) {}
    };
    template <class Parser> struct expect_failed_parse {
        int depth;
        int line, column;
        expect_failed_parse (int depth, int line, int column)
        : depth (depth), line (line), column (column) {}
    };

    std::shared_ptr <std::list <boost::any>> expected_list;
public:
    expect_observer() {
        expected_list = std::make_shared <std::list <boost::any>>();
    }

    ~expect_observer() {
        BOOST_CHECK ((!expected_list.unique()) || expected_list->empty());
    }

    template <class Parser>
    void expect_start (int depth, Parser const &, int line, int column) {
        expected_list->push_back (
            expect_start_parse <Parser> (depth, line, column));
    }
    template <class Parser>
    void expect_success (int depth, Parser const &,
        int start_line, int start_column, int finish_line, int finish_column)
    {
        expected_list->push_back (
            expect_successful_parse <Parser> (depth,
                start_line, start_column, finish_line, finish_column));
    }
    template <class Parser>
    void expect_no_success (int depth, Parser const &, int line, int column) {
        expected_list->push_back (
            expect_failed_parse <Parser> (depth, line, column));
    }

    template <class Parser, class Input>
        void start_parse (int depth, Parser const &, Input const & input)
            const
    {
        typedef expect_start_parse <Parser> expected_type;
        BOOST_REQUIRE (!expected_list->empty());
        boost::any got = expected_list->front();
        expected_type * expected = boost::any_cast <expected_type> (&got);
        BOOST_CHECK_MESSAGE (bool (expected),
            "Expected " << typeid (expected).name() << " but got "
            << got.type().name());
        if (expected) {
            BOOST_MESSAGE ("Found " << typeid (expected).name());
            std::cerr << std::flush;
            BOOST_CHECK_EQUAL (depth, expected->depth);
            BOOST_CHECK_EQUAL (input.line(), expected->line);
            BOOST_CHECK_EQUAL (input.column(), expected->column);
        }
        expected_list->pop_front();
    }

    template <class Parser, class Input, class Outcome>
        void success (int depth, Parser const & parser, Input const & input,
            Input const & rest, Outcome const &) const
    {
        typedef expect_successful_parse <Parser> expected_type;
        BOOST_REQUIRE (!expected_list->empty());
        boost::any got = expected_list->front();
        expected_type * expected = boost::any_cast <expected_type> (&got);
        BOOST_CHECK_MESSAGE (bool (expected),
            "Expected " << typeid (expected).name() << " but got "
            << got.type().name());
        if (expected) {
            BOOST_MESSAGE ("Found " << typeid (expected).name());
            BOOST_CHECK_EQUAL (depth, expected->depth);
            BOOST_CHECK_EQUAL (input.line(), expected->start_line);
            BOOST_CHECK_EQUAL (input.column(), expected->start_column);
            BOOST_CHECK_EQUAL (rest.line(), expected->finish_line);
            BOOST_CHECK_EQUAL (rest.column(), expected->finish_column);
        }
        expected_list->pop_front();
    }

    template <class Parser, class Input>
        void no_success (int depth, Parser const & parser, Input const & input)
    const
    {
        typedef expect_failed_parse <Parser> expected_type;
        BOOST_REQUIRE (!expected_list->empty());
        boost::any got = expected_list->front();
        expected_type * expected = boost::any_cast <expected_type> (&got);
        BOOST_CHECK_MESSAGE (bool (expected),
            "Expected " << typeid (expected).name() << " but got "
            << got.type().name());
        if (expected) {
            BOOST_MESSAGE ("Found " << typeid (expected).name());
            std::cerr << std::flush;
            BOOST_CHECK_EQUAL (depth, expected->depth);
            BOOST_CHECK_EQUAL (input.line(), expected->line);
            BOOST_CHECK_EQUAL (input.column(), expected->column);
        }
        expected_list->pop_front();
    }
};

auto string_inside = parse_ll::no_skip [
    +(parse_ll::char_ - parse_ll::one_whitespace)];
PARSE_LL_DEFINE_NAMED_PARSER (string2, string_inside)

auto one_a = literal ('a');
auto aaas = *one_a;
auto one_b = literal ('b');
auto inside_sequence = aaas >> one_b;
auto outside_sequence = inside_sequence >> string2;


BOOST_AUTO_TEST_CASE (test_trace_automatic) {
    expect_observer observer;

/*
sequence (1,1)
 sequence (1,1)
  repeat (1,1)
   literal (1,1)
   literal successful (1,2) "a"
   literal (1,2)
   literal successful (1,3) "a"
   literal (1,3)
   literal failed
  repeat successful (1,3) "aa"
  literal (1,3)
  literal successful (1,4) "b"
 sequence successful (1,4) "aab"
 string2 (1,4)
  (policy change inside) (1,4)
  (policy change inside) successful (1,6) "st"
 string2 successful (1,6) "st"
sequence successful (1,6) "aabst"
*/
    observer.expect_start (0, outside_sequence, 0, 0);
        observer.expect_start (1, inside_sequence, 0, 0);
            observer.expect_start (2, aaas, 0, 0);
                observer.expect_start (3, one_a, 0, 0);
                observer.expect_success (3, one_a, 0, 0, 0, 1);
                observer.expect_start (3, one_a, 0, 1);
                observer.expect_success (3, one_a, 0, 1, 0, 2);
                observer.expect_start (3, one_a, 0, 2);
                observer.expect_no_success (3, one_a, 0, 2);
            observer.expect_success (2, aaas, 0, 0, 0, 2);
            observer.expect_start (2, one_b, 0, 2);
            observer.expect_success (2, one_b, 0, 2, 0, 3);
        observer.expect_success (1, inside_sequence, 0, 0, 0, 3);
        observer.expect_start (1, string2, 0, 3);
            observer.expect_start (2, string_inside, 0, 3);
            observer.expect_success (2, string_inside, 0, 3, 0, 5);
        observer.expect_success (1, string2, 0, 3, 0, 5);
    observer.expect_success (0, outside_sequence, 0, 0, 0, 5);

    auto parser = trace (observer, parse_ll::parse_policy::stop_at_no_skip()) [
        outside_sequence];
    std::string s = "aabst";

    auto input1 = range::view (s);
    range::text_location_range <decltype (input1)> input (input1);

    parse (parser, input);

//    BOOST_CHECK (false);
}

BOOST_AUTO_TEST_SUITE_END()

