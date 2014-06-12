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
Test rule.
*/

#define BOOST_TEST_MODULE rule_parser
#include "utility/test/boost_unit_test.hpp"

#include "parse_ll/core/rule.hpp"

#include <string>

#include "range/core.hpp"
#include "range/std/container.hpp"

#include "parse_ll/core/char.hpp"
#include "parse_ll/core/nothing.hpp"
#include "parse_ll/core/transform.hpp"
#include "parse_ll/core/sequence.hpp"

#include <boost/phoenix/core/value.hpp>
#include "../helper/object.hpp"
#include "../helper/fuzz_parser.hpp"

#include "parse_ll/core/whitespace.hpp"
#include "parse_ll/core/skip.hpp"
#include "parse_ll/core/no_skip.hpp"

BOOST_AUTO_TEST_SUITE(test_parse_rule)

BOOST_AUTO_TEST_CASE (test_rule) {
    using range::empty;

    using parse_ll::parse;
    using parse_ll::success;
    using parse_ll::output;
    using parse_ll::rest;

    using boost::phoenix::val;
    typedef range::result_of <range::callable::view (std::string &)>::type
        input_type;
    {
        std::string r ("");
        {
            // Can be default-initialised.
            parse_ll::rule <input_type> parser;
            parser = parse_ll::char_ ('a') >> parse_ll::char_ ('b');
            auto result = parse (parser, r);
            BOOST_CHECK (!success (result));
        }
        {
            parse_ll::rule <input_type> parser = parse_ll::nothing;
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            output (result);
        }
    }
    {
        std::string r ("a");
        {
            parse_ll::rule <input_type, std::tuple <char, char> > parser
                = fuzz (parse_ll::char_ ('a') >> parse_ll::char_ ('b'));
            auto result = parse (parser, r);
            BOOST_CHECK (!success (result));
        }
        {
            parse_ll::rule <input_type, std::tuple <char, char, char> > parser
                = fuzz (parse_ll::nothing [boost::phoenix::val ('c')]
                    >> parse_ll::char_ ('a') >> parse_ll::char_ ('b'));
            auto result = parse (parser, r);
            BOOST_CHECK (!success (result));
        }
    }
    {
        std::string r ("ab");
        {
            auto parser = fuzz (
                parse_ll::rule <input_type, std::tuple <int, int> > (
                    parse_ll::char_ ('a') [val (1)]
                    >> parse_ll::char_ ('b') [val (2)]));
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            auto o = output (result);
            BOOST_CHECK_EQUAL (std::get <0> (o), 1);
            BOOST_CHECK_EQUAL (std::get <1> (o), 2);
            BOOST_CHECK (empty (rest (result)));
        }
        // Variant with an internal rule
        {
            parse_ll::rule <input_type, char> b_parser
                = fuzz (parse_ll::char_ ('b'));
            parse_ll::rule <input_type, std::tuple <int, int> > parser
                = parse_ll::char_ ('a') [val (1)] >> fuzz (b_parser [val (2)]);
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
            parse_ll::rule <input_type, int> b_parser
                = parse_ll::char_ ('b') [val (2)];
            parse_ll::rule <input_type, std::tuple <int, int> > parser
                = parse_ll::char_ ('a') [val (1)] >> fuzz (b_parser);
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            auto o = output (result);
            BOOST_CHECK_EQUAL (std::get <0> (o), 1);
            BOOST_CHECK_EQUAL (std::get <1> (o), 2);
            BOOST_CHECK (empty (rest (result)));
        }
        {
            parse_ll::rule <input_type, std::tuple <int, int> > parser
                = parse_ll::char_ ('b') [val (2)]
                    >> parse_ll::char_ ('a') [val (1)];
            auto result = parse (parser, r);
            BOOST_CHECK (!success (result));
        }
        {
            parse_ll::rule <input_type, object> b_parser
                = parse_ll::char_ ('b') [always (object (1))];
            parse_ll::rule <input_type, std::tuple <
                object, object, object> > parser
                    = parse_ll::char_ ('a') [always (object (2))]
                    >> b_parser
                    >> parse_ll::nothing [always (object (3))];
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            auto o = output (result);
            BOOST_CHECK_EQUAL (std::get <0> (o), 2);
            BOOST_CHECK_EQUAL (std::get <1> (o), 1);
            BOOST_CHECK_EQUAL (std::get <2> (o), 3);
            BOOST_CHECK (empty (rest (result)));
        }
    }
}

// Test propagation of skip parser.
BOOST_AUTO_TEST_CASE (test_rule_skip_parser) {
    using range::empty;

    using parse_ll::parse;
    using parse_ll::success;
    using parse_ll::output;
    using parse_ll::rest;

    using boost::phoenix::val;
    typedef range::result_of <range::callable::view (std::string &)>::type
        input_type;
    {
        std::string r ("a b");
        {
            // Polymorphic skip parser.
            parse_ll::rule <input_type> parser =
                parse_ll::char_ ('a') >> fuzz (parse_ll::char_ ('b'));
            {
                auto result = parse (parser, r);
                BOOST_CHECK (!success (result));
            }
            {
                auto result = parse (
                    parse_ll::skip (parse_ll::whitespace) [parser], r);
                BOOST_CHECK (success (result));
            }
            {
                auto result = parse (
                    parse_ll::skip (fuzz (parse_ll::one_horizontal_space)) [
                        parser], r);
                BOOST_CHECK (success (result));
            }
        }
        {
            // Propagate a given skip parser.
            auto skip_parser = parse_ll::whitespace;
            parse_ll::rule <input_type, void, decltype (skip_parser)> parser =
                parse_ll::char_ ('a') >> parse_ll::char_ ('b');
            /*{
                // This fails at compile time because the skip parser is of the
                // wrong type.
                auto result = parse (parser, r);
            }*/
            {
                auto p = fuzz (
                    parse_ll::skip (parse_ll::whitespace) [parser]);
                auto result = parse (p, r);
                BOOST_CHECK (success (result));
            }
        }
        {
            // Promise to use a different skip parser inside.
            parse_ll::rule <input_type, void,
                parse_ll::rule_explicit_skip_parser> parser =
                    fuzz (parse_ll::no_skip [
                        parse_ll::char_ ('a') >> parse_ll::char_ ('b')]);
            {
                auto result = parse (parser, r);
                BOOST_CHECK (!success (result));
            }
            {
                auto p = parse_ll::skip (parse_ll::whitespace) [fuzz (parser)];
                auto result = parse (p, r);
                BOOST_CHECK (!success (result));
            }
        }
        {
            parse_ll::rule <input_type, void,
                parse_ll::rule_explicit_skip_parser> parser =
                    parse_ll::skip (fuzz (parse_ll::whitespace)) [
                        parse_ll::char_ ('a') >> parse_ll::char_ ('b')];
            {
                auto p = fuzz (parser);
                auto result = parse (p, r);
                BOOST_CHECK (success (result));
            }
            {
                auto p = fuzz (parse_ll::skip (parse_ll::whitespace) [parser]);
                auto result = parse (p, r);
                BOOST_CHECK (success (result));
            }
            {
                auto p = parse_ll::skip (parse_ll::char_ ('a')) [parser];
                auto result = parse (p, r);
                BOOST_CHECK (success (result));
            }
        }
        /*{
            parse_ll::rule <input_type, void,
                parse_ll::rule_explicit_skip_parser> parser =
                    parse_ll::char_ ('a') >> parse_ll::char_ ('b');
            {
                // Compile error because there is no skip parser inside.
                auto result = parse (parser, r);
            }
        }*/
    }
}

BOOST_AUTO_TEST_SUITE_END()

