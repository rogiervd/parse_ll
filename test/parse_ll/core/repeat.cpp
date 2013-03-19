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
Test repeat parser.
*/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE repeat_parser
#include <boost/test/unit_test.hpp>

#include "parse_ll/core/repeat.hpp"

#include <string>

#include "range/core.hpp"
#include "range/std/container.hpp"

#include "parse_ll/core/literal.hpp"
#include "parse_ll/core/char.hpp"

#include "parse_ll/core/transform.hpp"
#include "../helper/object.hpp"
#include "../helper/fuzz_parser.hpp"

#include "parse_ll/core/no_skip.hpp"
#include "parse_ll/core/skip.hpp"

BOOST_AUTO_TEST_SUITE(test_parse_repeat)

BOOST_AUTO_TEST_CASE (test_repeat) {
    using range::empty; using range::first; using range::drop;

    using parse_ll::parse;
    using parse_ll::success;
    using parse_ll::output;
    using parse_ll::rest;
    {
        std::string r ("");
        {
            auto parser = *parse_ll::char_ ('a');
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            BOOST_CHECK (empty (output (result)));
            BOOST_CHECK (empty (rest (result)));
        }
        {
            auto parser = parse_ll::repeat (0, 3) [parse_ll::char_ ('a')];
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            BOOST_CHECK (empty (output (result)));
            BOOST_CHECK (empty (rest (result)));
        }
        {
            auto parser = parse_ll::repeat (1) [parse_ll::char_ ('a')];
            auto result = parse (parser, r);
            BOOST_CHECK (!success (result));
        }
        {
            auto parser = parse_ll::repeat (1, 3) [parse_ll::char_ ('a')];
            auto result = parse (parser, r);
            BOOST_CHECK (!success (result));
        }
    }

    {
        std::string r ("a");
        {
            auto parser = fuzz (*parse_ll::char_ ('a'));
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            BOOST_CHECK_EQUAL (first (output (result)), 'a');
            BOOST_CHECK (empty (drop (output (result))));
            BOOST_CHECK (empty (rest (result)));
        }
        {
            // Test void return type.
            auto parser = fuzz (*parse_ll::literal ('a'));
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            static_assert (
                std::is_same <decltype (output (result)), void>::value,
                "Repeating a parser with void output should have void output");
            BOOST_CHECK (empty (rest (result)));
        }
        {
            auto parser = fuzz (parse_ll::repeat (0, 3) [
                fuzz (parse_ll::char_ ('a'))]);
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            BOOST_CHECK_EQUAL (first (output (result)), 'a');
            BOOST_CHECK (empty (drop (output (result))));
            BOOST_CHECK (empty (rest (result)));
        }
        {
            auto parser = parse_ll::repeat (1) [fuzz (parse_ll::char_ ('a'))];
            auto result = parse (parser, r);
            BOOST_CHECK_EQUAL (first (output (result)), 'a');
            BOOST_CHECK (empty (drop (output (result))));
            BOOST_CHECK (empty (rest (result)));
        }
        {
            auto parser = fuzz (parse_ll::repeat.at_least (2) [
                fuzz (parse_ll::char_ ('a'))]);
            auto result = parse (parser, r);
            BOOST_CHECK (!success (result));
        }
    }
    {
        std::string r ("ab");
        {
            auto parser = *parse_ll::char_ ('a');
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            BOOST_CHECK_EQUAL (first (output (result)), 'a');
            BOOST_CHECK (empty (drop (output (result))));
            BOOST_CHECK_EQUAL (first (rest (result)), 'b');
        }
        {
            auto parser = parse_ll::repeat (0, 3) [parse_ll::char_ ('a')];
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            BOOST_CHECK_EQUAL (first (output (result)), 'a');
            BOOST_CHECK (empty (drop (output (result))));
            BOOST_CHECK_EQUAL (first (rest (result)), 'b');
        }
        {
            auto parser = parse_ll::repeat (2) [parse_ll::char_ ('a')];
            auto result = parse (parser, r);
            BOOST_CHECK (!success (result));
        }
        {
            auto parser = parse_ll::repeat.at_least (1) [parse_ll::char_ ('a')];
            auto result = parse (parser, r);
            BOOST_CHECK_EQUAL (first (output (result)), 'a');
            BOOST_CHECK (empty (drop (output (result))));
            BOOST_CHECK_EQUAL (first (rest (result)), 'b');
        }
        {
            auto parser = *parse_ll::char_ ;
            auto result = parse (parser, r);
            BOOST_CHECK_EQUAL (first (output (result)), 'a');
            BOOST_CHECK_EQUAL (first (drop (output (result))), 'b');
            BOOST_CHECK (empty (drop (drop (output (result)))));
            BOOST_CHECK (empty (rest (result)));
        }
    }
    {
        std::string r ("aaab");
        {
            auto parser = *parse_ll::char_ ('a');
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            BOOST_CHECK_EQUAL (first (output (result)), 'a');
            BOOST_CHECK_EQUAL (first (drop (output (result))), 'a');
            BOOST_CHECK_EQUAL (first (drop (drop (output (result)))), 'a');
            BOOST_CHECK (empty (drop (drop (drop (output (result))))));
            BOOST_CHECK_EQUAL (first (rest (result)), 'b');
        }
        {
            auto parser = parse_ll::repeat (1, 3) [parse_ll::char_ ('a')];
            auto result = parse (parser, r);
            BOOST_CHECK_EQUAL (first (output (result)), 'a');
            BOOST_CHECK_EQUAL (first (drop (output (result))), 'a');
            BOOST_CHECK_EQUAL (first (drop (drop (output (result)))), 'a');
            BOOST_CHECK (empty (drop (drop (drop (output (result))))));
            BOOST_CHECK_EQUAL (first (rest (result)), 'b');
        }
        {
            auto parser = fuzz (
                parse_ll::repeat (0, 2) [parse_ll::char_ ('a')]);
            {
                auto result = parse (parser, r);
                BOOST_CHECK_EQUAL (first (output (result)), 'a');
                BOOST_CHECK_EQUAL (first (drop (output (result))), 'a');
                BOOST_CHECK (empty (drop (drop (output (result)))));
                BOOST_CHECK_EQUAL (first (rest (result)), 'a');
                BOOST_CHECK_EQUAL (first (drop (rest (result))), 'b');
            }
            {
                auto o = output (parse (parser, r));
                BOOST_CHECK_EQUAL (first (o), 'a');
                BOOST_CHECK_EQUAL (first (drop (o)), 'a');
                BOOST_CHECK (empty (drop (drop (o))));
            }
            {
                auto re = rest (parse (parser, r));
                BOOST_CHECK_EQUAL (first (re), 'a');
                BOOST_CHECK_EQUAL (first (drop (re)), 'b');
            }
        }
        {
            auto parser = parse_ll::repeat (2) [fuzz (parse_ll::char_ ('a'))];
            auto result = parse (parser, r);
            BOOST_CHECK_EQUAL (first (output (result)), 'a');
            BOOST_CHECK_EQUAL (first (drop (output (result))), 'a');
            BOOST_CHECK (empty (drop (drop (output (result)))));
            BOOST_CHECK_EQUAL (first (rest (result)), 'a');
            BOOST_CHECK_EQUAL (first (drop (rest (result))), 'b');
        }
        {
            auto parser = parse_ll::repeat.at_least (1) [parse_ll::char_ ('a')];
            auto result = parse (parser, r);
            BOOST_CHECK_EQUAL (first (output (result)), 'a');
            BOOST_CHECK_EQUAL (first (drop (output (result))), 'a');
            BOOST_CHECK_EQUAL (first (drop (drop (output (result)))), 'a');
            BOOST_CHECK (empty (drop (drop (drop (output (result))))));
            BOOST_CHECK_EQUAL (first (rest (result)), 'b');
        }
        // Copying parsers and parses
        {
            auto parser = *(parse_ll::char_ ('a') [always (object (78))]);
            {
                auto result = parse (parser, r);
                BOOST_CHECK (success (result));
                BOOST_CHECK_EQUAL (first (output (result)), 78);
                BOOST_CHECK_EQUAL (first (drop (output (result))), 78);
                BOOST_CHECK_EQUAL (first (drop (drop (output (result)))), 78);
                BOOST_CHECK (empty (drop (drop (drop (output (result))))));
                BOOST_CHECK_EQUAL (first (rest (result)), 'b');
            }
            {
                auto o = output (parse (parser, r));
                BOOST_CHECK_EQUAL (first (o), 78);
                BOOST_CHECK_EQUAL (first (drop (o)), 78);
                BOOST_CHECK_EQUAL (first (drop (drop (o))), 78);
                BOOST_CHECK (empty (drop (drop (drop (o)))));
            }
        }
    }
}

BOOST_AUTO_TEST_CASE (test_repeat_skipping) {
    using range::empty; using range::first; using range::drop;

    using parse_ll::parse;
    using parse_ll::success;
    using parse_ll::output;
    using parse_ll::rest;
    using parse_ll::no_skip;
    using parse_ll::skip;
    using parse_ll::literal;

    auto skip_space = skip (literal (' '));
    {
        std::string r ("a");
        {
            auto parser = skip_space [*parse_ll::char_ ('a')];
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            BOOST_CHECK (empty (drop (output (result))));
            BOOST_CHECK (empty (rest (result)));
        }
    }
    {
        std::string r (" a");
        {
            auto parser = skip_space [*parse_ll::char_ ('a')];
            auto result = parse (parser, r);
            // Success but nothing parsed.
            BOOST_CHECK (success (result));
            BOOST_CHECK (empty (output (result)));
        }
    }
    {
        std::string r ("a ");
        {
            auto parser = skip_space [*parse_ll::char_ ('a')];
            auto result = parse (parser, r);
            // Success: one "a" parsed, not the " " following.
            BOOST_CHECK (success (result));
            BOOST_CHECK (!empty (output (result)));
            BOOST_CHECK (empty (drop (output (result))));
            BOOST_CHECK (!empty (rest (result)));
            BOOST_CHECK_EQUAL (first (rest (result)), ' ');
        }
    }
    {
        // Test parser with single spaces between groups and commas within.
        auto group_parser = skip (literal (',')) [+fuzz (parse_ll::char_('a'))];
        auto parser = fuzz (skip_space [*fuzz (group_parser)]);
        {
            std::string r ("a a,a,a a,a");
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));

            auto o = output (result);
            BOOST_CHECK (!empty (o));
            BOOST_CHECK (!empty (drop (o)));
            BOOST_CHECK (!empty (drop (drop (o))));
            auto number2 = drop (drop (o));
            auto number3 = drop (number2);
            BOOST_CHECK (empty (number3));
            auto group1 = first (o);
            auto group2 = first (drop (o));
            auto group3 = first (drop (drop (o)));

            BOOST_CHECK (empty (drop (group1)));
            BOOST_CHECK (empty (drop (drop (drop (group2)))));
            BOOST_CHECK (empty (drop (drop (group3))));

            BOOST_CHECK (empty (rest (result)));
        }
        // The space can occur only between groups.
        {
            std::string r (" a a,a");
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            BOOST_CHECK (empty (output (result)));
            BOOST_CHECK_EQUAL (first (rest (result)), ' ');
        }
        // The comma can occur only within groups of 'a'.
        {
            std::string r (",a");
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            BOOST_CHECK (empty (output (result)));
            BOOST_CHECK_EQUAL (first (rest (result)), ',');
        }
        {
            std::string r ("a ,a");
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            BOOST_CHECK_EQUAL (first (rest (result)), ' ');
        }
        {
            std::string r ("a a,");
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            BOOST_CHECK_EQUAL (first (rest (result)), ',');
        }
        {
            std::string r ("a aa,");
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            // Should have parsed up to the last ','.
            BOOST_CHECK_EQUAL (first (rest (result)), ',');
            auto group1 = first (output (result));
            auto group2 = first (drop (output (result)));
            BOOST_CHECK (empty (drop (drop (output (result)))));
            BOOST_CHECK (empty (drop (group1)));
            BOOST_CHECK (empty (drop (drop (group2))));
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()

