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
Test sequence parser.
*/

#define BOOST_TEST_MODULE sequence_parser
#include "utility/test/boost_unit_test.hpp"

#include "parse_ll/core/sequence.hpp"

#include <string>

#include "range/core.hpp"
#include "range/std/container.hpp"

#include "parse_ll/core/nothing.hpp"
#include "parse_ll/core/char.hpp"
#include "parse_ll/core/literal.hpp"
#include "parse_ll/core/transform.hpp"

#include <boost/phoenix/core/value.hpp>
#include "../helper/object.hpp"
#include "../helper/fuzz_parser.hpp"

#include "parse_ll/core/no_skip.hpp"
#include "parse_ll/core/skip.hpp"

BOOST_AUTO_TEST_SUITE(test_parse_sequence)

BOOST_AUTO_TEST_CASE (test_sequence) {
    using range::empty;

    using parse_ll::parse;
    using parse_ll::success;
    using parse_ll::output;
    using parse_ll::rest;

    using boost::phoenix::val;
    {
        std::string r ("");
        {
            auto parser = parse_ll::char_ ('a') >> parse_ll::char_ ('b');
            auto result = parse (parser, r);
            BOOST_CHECK (!success (result));
        }
        {
            auto parser = (parse_ll::nothing [boost::phoenix::val ('c')]
                >> parse_ll::char_ ('a')) >> parse_ll::char_ ('b');
            auto result = parse (parser, r);
            BOOST_CHECK (!success (result));
        }
    }
    {
        std::string r ("a");
        {
            auto parser = fuzz (fuzz (parse_ll::char_ ('a'))
                >> fuzz (parse_ll::char_ ('b')));
            auto result = parse (parser, r);
            BOOST_CHECK (!success (result));
        }
        {
            auto parser = fuzz (parse_ll::nothing [boost::phoenix::val ('c')])
                >> fuzz (parse_ll::char_ ('a')) >> fuzz (parse_ll::char_ ('b'));
            auto result = parse (parser, r);
            BOOST_CHECK (!success (result));
        }
    }
    {
        std::string r ("ab");
        {
            auto parser = fuzz (fuzz (parse_ll::char_ ('a') [val (1)])
                >> fuzz (parse_ll::char_ ('b') [val (2)]));
            {
                auto result = parse (parser, r);
                BOOST_CHECK (success (result));
                auto o = output (result);
                BOOST_CHECK_EQUAL (std::get <0> (o), 1);
                BOOST_CHECK_EQUAL (std::get <1> (o), 2);
                BOOST_CHECK (empty (rest (result)));
            }
            {
                auto o = output (parse (parser, r));
                BOOST_CHECK_EQUAL (std::get <0> (o), 1);
                BOOST_CHECK_EQUAL (std::get <1> (o), 2);
            }
        }
        {
            auto parser = parse_ll::char_ ('b') [val (2)]
                >> parse_ll::char_ ('a') [val (1)];
            auto result = parse (parser, r);
            BOOST_CHECK (!success (result));
        }
        {
            auto parser = fuzz (
                parse_ll::char_ ('a') [always (object (2))]
                >> parse_ll::char_ ('b') [always (object (1))]
                >> parse_ll::nothing [always (object (3))]);
            {
                auto result = parse (parser, r);
                BOOST_CHECK (success (result));
                auto o = output (result);
                BOOST_CHECK_EQUAL (std::get <0> (o), 2);
                BOOST_CHECK_EQUAL (std::get <1> (o), 1);
                BOOST_CHECK_EQUAL (std::get <2> (o), 3);
                BOOST_CHECK (empty (rest (result)));
            }
            {
                auto o = output (parse (parser, r));
                BOOST_CHECK_EQUAL (std::get <0> (o), 2);
                BOOST_CHECK_EQUAL (std::get <1> (o), 1);
                BOOST_CHECK_EQUAL (std::get <2> (o), 3);
            }
        }
        // void output type on sub-parsers
        {
            auto parser =
                parse_ll::char_ ('a') [always (object (2))]
                >> parse_ll::char_ ('b') [always()]
                >> parse_ll::nothing [always (object (3))];
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            auto o = output (result);
            static_assert (std::tuple_size <decltype (o)>::value == 2,
                "The result type must have only 2 elements, "
                "because one sub-parser outputs void");
            BOOST_CHECK_EQUAL (std::get <0> (o), 2);
            BOOST_CHECK_EQUAL (std::get <1> (o), 3);
            BOOST_CHECK (empty (rest (result)));
        }
        {
            auto parser =
                parse_ll::char_ ('a') [always()]
                >> parse_ll::char_ ('b') [always (object (2))]
                >> parse_ll::nothing [always (object (3))];
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            auto o = output (result);
            static_assert (std::tuple_size <decltype (o)>::value == 2,
                "The result type must have only 2 elements, "
                "because one sub-parser outputs void");
            BOOST_CHECK_EQUAL (std::get <0> (o), 2);
            BOOST_CHECK_EQUAL (std::get <1> (o), 3);
            BOOST_CHECK (empty (rest (result)));
        }
        {
            auto parser =
                fuzz (parse_ll::char_ ('a') [always(object (2))])
                >> parse_ll::char_ ('b') [always (object (3))]
                >> fuzz (parse_ll::nothing [always()]);
            {
                auto result = parse (parser, r);
                BOOST_CHECK (success (result));
                auto o = output (result);
                static_assert (std::tuple_size <decltype (o)>::value == 2,
                    "The result type must have only 2 elements, "
                    "because one sub-parser outputs void");
                BOOST_CHECK_EQUAL (std::get <0> (o), 2);
                BOOST_CHECK_EQUAL (std::get <1> (o), 3);
                BOOST_CHECK (empty (rest (result)));
            }
            {
                auto o = output (parse (parser, r));
                BOOST_CHECK_EQUAL (std::get <0> (o), 2);
                BOOST_CHECK_EQUAL (std::get <1> (o), 3);
            }
        }
    }
}

BOOST_AUTO_TEST_CASE (test_expect_sequence) {
    using range::empty; using range::first; using range::drop;

    using parse_ll::parse;
    using parse_ll::success;
    using parse_ll::output;
    using parse_ll::rest;

    using boost::phoenix::val;
    {
        std::string r ("");
        {
            auto parser = parse_ll::char_ ('a') > parse_ll::char_ ('b');
            auto result = parse (parser, r);
            BOOST_CHECK (!success (result));
        }
        {
            auto parser = parse_ll::nothing [boost::phoenix::val ('c')]
                > parse_ll::char_ ('a') > parse_ll::char_ ('b');
            // This must be successful, since "nothing" is successful.
//            BOOST_CHECK (success (result));
            BOOST_CHECK_THROW (output (parse (parser, r)), parse_ll::error);
        }
    }
    {
        std::string r ("a");
        {
            auto parser = parse_ll::char_ ('a') > parse_ll::char_ ('b');
            BOOST_CHECK_THROW (output (parse (parser, r)), parse_ll::error);
        }
        {
            auto parser = (parse_ll::nothing [boost::phoenix::val ('c')]
                >> parse_ll::char_ ('a')) > parse_ll::char_ ('b');
            BOOST_CHECK_THROW (output (parse (parser, r)), parse_ll::error);
        }
    }
    {
        std::string r ("ab");
        {
            auto parser = fuzz (fuzz (parse_ll::char_ ('a'))
                > fuzz (parse_ll::char_ ('b')));
            {
                auto result = parse (parser, r);
                BOOST_CHECK (success (result));
                BOOST_CHECK_EQUAL (std::get <0> (output (result)), 'a');
                BOOST_CHECK_EQUAL (std::get <1> (output (result)), 'b');
                BOOST_CHECK (empty (rest (result)));
            }
            {
                auto o = output (parse (parser, r));
                BOOST_CHECK_EQUAL (std::get <0> (o), 'a');
                BOOST_CHECK_EQUAL (std::get <1> (o), 'b');
            }
        }
        {
            auto parser = parse_ll::nothing [always (object (5))]
                >> parse_ll::char_ ('a') [always (object (7))]
                > parse_ll::char_ ('b') [always (object (9))];
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            BOOST_CHECK_EQUAL (std::get <0> (output (result)), 5);
            BOOST_CHECK_EQUAL (std::get <1> (output (result)), 7);
            BOOST_CHECK_EQUAL (std::get <2> (output (result)), 9);
            BOOST_CHECK (empty (rest (result)));
        }
        {
            // \todo Fix parentheses for precedence when a linear
            // version of sequence is in.
            auto parser = fuzz ((fuzz (parse_ll::nothing [always (object (5))])
                > fuzz (parse_ll::char_ ('a') [always (object (7))]))
                >> fuzz (parse_ll::char_ ('b') [always (object (9))]));
            {
                auto result = parse (parser, r);
                BOOST_CHECK (success (result));
                BOOST_CHECK_EQUAL (std::get <0> (output (result)), 5);
                BOOST_CHECK_EQUAL (std::get <1> (output (result)), 7);
                BOOST_CHECK_EQUAL (std::get <2> (output (result)), 9);
                BOOST_CHECK (empty (rest (result)));
            }
            {
                auto o = output (parse (parser, r));
                BOOST_CHECK_EQUAL (std::get <0> (o), 5);
                BOOST_CHECK_EQUAL (std::get <1> (o), 7);
                BOOST_CHECK_EQUAL (std::get <2> (o), 9);
            }
        }
        {
            auto parser = parse_ll::char_ ('a') [always (object (7))]
                > parse_ll::char_ ('b') [always (object (9))]
                > parse_ll::char_ ('c') [always (object (11))];
            try {
                auto result = parse (parser, r);
                BOOST_CHECK (success (result));
                std::get <2> (output (result));
                // Should have thrown!
                BOOST_CHECK (false);
            } catch (parse_ll::error & e) {
                typedef decltype (range::view (r)) input_type;
                input_type * position = boost::get_error_info <
                    parse_ll::error_position <input_type>::type> (e);
                BOOST_CHECK (position);
                if (position)
                    BOOST_CHECK (empty (*position));
            }
        }
        {   // Check that the error points to the last ">".
            // This is a design issue under consideration.

            // \todo Fix parentheses for precedence when a linear
            // version of sequence is in.
            auto parser = (parse_ll::char_ ('a') [always (object (7))]
                > parse_ll::char_ ('b') [always (object (9))])
                >> parse_ll::char_ ('c') [always (object (11))];
            // Currently, this does not throw: a > b succeeds, and .. >> c
            // simply doesn't.
            auto result = parse (parser, r);
            BOOST_CHECK (!success (result));
            /*try {
                auto result = parse (parser, r);
                BOOST_CHECK (success (result));
                std::get <2> (output (result));
                // Should have thrown!
                BOOST_CHECK (false);
            } catch (parse_ll::error_at <decltype (range::view (r))> & e) {
                BOOST_CHECK_EQUAL (first (e.position()), 'b');
            }*/
        }
        {
            auto parser = parse_ll::char_ ('a') [always (object (7))]
                > parse_ll::char_ ('d') [always (object (9))]
                > parse_ll::char_ ('c') [always (object (11))];
            try {
                auto result = parse (parser, r);
                BOOST_CHECK (success (result));
                std::get <2> (output (result));
                // Should have thrown!
                BOOST_CHECK (false);
            } catch (parse_ll::error & e) {
                typedef decltype (range::view (r)) input_type;
                input_type * position = boost::get_error_info <
                    parse_ll::error_position <input_type>::type> (e);
                BOOST_CHECK (position);
                if (position)
                    BOOST_CHECK_EQUAL (first (*position), 'b');
            }
        }
    }
}

BOOST_AUTO_TEST_CASE (test_sequence_skipping) {
    using range::empty;
    using range::first;
    using range::drop;

    using parse_ll::parse;
    using parse_ll::success;
    using parse_ll::output;
    using parse_ll::rest;
    using parse_ll::no_skip;
    using parse_ll::skip;
    using parse_ll::char_;
    using parse_ll::literal;

    auto skip_space = skip (literal (' '));
    {
        std::string r ("a a");
        {
            auto parser = skip_space [char_ ('a') >> char_ ('a')];
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            auto o = output (result);
            static_assert (std::tuple_size <decltype (o)>::value == 2, "");
            BOOST_CHECK (empty (rest (result)));
        }
    }
    {
        std::string r (" a a");
        {
            auto parser = skip_space [char_ ('a') >> char_ ('a')];
            auto result = parse (parser, r);
            BOOST_CHECK (!success (result));
        }
    }
    {
        std::string r ("a a ");
        {
            auto parser = skip_space [char_ ('a') >> char_ ('a')];
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            static_assert (
                std::tuple_size <decltype (output (result))>::value == 2, "");
            BOOST_CHECK_EQUAL (first (rest (result)), ' ');
        }
    }
    {
        // Test parser with single spaces between groups and commas within.
        // This parses "a,a a,a" where the space and the commas are optional.
        auto group_parser =
            skip (literal (',')) [fuzz (char_('a') >> char_('a'))];
        auto parser = fuzz (skip_space [
            fuzz (group_parser) >> fuzz (group_parser)]);
        {
            std::string r ("aa a,a,");
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));

            BOOST_CHECK_EQUAL (first (rest (result)), ',');
        }
        {
            std::string r ("aaaa ");
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));

            BOOST_CHECK_EQUAL (first (rest (result)), ' ');
        }
        // The space can occur only between groups.
        {
            std::string r ("a,a a,a ");
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));

            BOOST_CHECK_EQUAL (first (rest (result)), ' ');
        }
        {
            std::string r (" a,a a,a");
            auto result = parse (parser, r);
            BOOST_CHECK (!success (result));
        }
        // The comma can occur only within groups of 'a'.
        {
            std::string r (",a,a a,a");
            auto result = parse (parser, r);
            BOOST_CHECK (!success (result));
        }
        {
            std::string r ("a,a, a,a");
            auto result = parse (parser, r);
            BOOST_CHECK (!success (result));
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()

