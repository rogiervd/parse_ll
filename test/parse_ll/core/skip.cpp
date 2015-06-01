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
Test skip and no_skip.
*/

#define BOOST_TEST_MODULE skip_no_skip
#include "utility/test/boost_unit_test.hpp"

#include "parse_ll/core/no_skip.hpp"
#include "parse_ll/core/skip.hpp"

#include <string>

#include "range/core.hpp"
#include "range/std/container.hpp"

#include "parse_ll/core/core.hpp"
#include "parse_ll/core/literal.hpp"
#include "parse_ll/core/outcome.hpp"

#include "../helper/fuzz_parser.hpp"

/**
Silly parser that just uses the skip parser.
*/
struct silly_skip_parser : parse_ll::parser_base <silly_skip_parser>
{ silly_skip_parser() {} };

struct silly_skip_parser_tag;

namespace parse_ll {
    template <> struct decayed_parser_tag <silly_skip_parser>
    { typedef silly_skip_parser_tag type; };
} // namespace parse_ll

static const auto silly_skip = silly_skip_parser();

namespace parse_ll { namespace operation {

    template <> struct parse <silly_skip_parser_tag> {
        template <class Policy, class Input>
            parse_ll::explicit_outcome <void, Input>
                operator() (Policy const & policy, silly_skip_parser const &,
                    Input const & input) const
        {
            return explicit_outcome <void, Input> (
                parse_ll::skip_over (policy.skip_parser(), input));
        }
    };

}} // namespace parse_ll::operation

BOOST_AUTO_TEST_SUITE(test_parse_skip)

BOOST_AUTO_TEST_CASE (test_skip) {
    using range::empty; using range::first; using range::drop;

    using parse_ll::parse;
    using parse_ll::success;
    using parse_ll::output;
    using parse_ll::rest;
    {
        std::string r (" ");
        {
            auto parser = silly_skip;
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            BOOST_CHECK_EQUAL (first (rest (result)), ' ');
        }
        {
            auto parser = parse_ll::skip (parse_ll::literal (' ')) [silly_skip];
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            BOOST_CHECK (empty (rest (result)));
        }
        {
            auto parser = fuzz (parse_ll::skip (
                fuzz (parse_ll::literal (' '))) [fuzz (silly_skip)]);
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            BOOST_CHECK (empty (rest (result)));
        }
        {
            // Add no_skip outside -- should not make a difference.
            auto parser = parse_ll::no_skip [fuzz (parse_ll::skip (
                fuzz (parse_ll::literal (' '))) [fuzz (silly_skip)])];
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            BOOST_CHECK (empty (rest (result)));
        }
        {
            auto parser = fuzz (parse_ll::no_skip [fuzz (parse_ll::skip (
                fuzz (parse_ll::literal (' '))) [fuzz (silly_skip)])]);
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            BOOST_CHECK (empty (rest (result)));
        }
        {
            // no_skip inside; this inhibits skipping again.
            auto parser = fuzz (parse_ll::skip (
                fuzz (parse_ll::literal (' '))) [fuzz (
                    parse_ll::no_skip [silly_skip])]);
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            BOOST_CHECK_EQUAL (first (rest (result)), ' ');
        }
        {
            auto parser = parse_ll::no_skip [fuzz (parse_ll::skip (
                fuzz (parse_ll::literal (' '))) [fuzz (
                    parse_ll::no_skip [silly_skip])])];
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            BOOST_CHECK_EQUAL (first (rest (result)), ' ');
        }
        {
            // Nested no_skip.
            auto parser = parse_ll::no_skip [fuzz (parse_ll::skip (
                fuzz (parse_ll::literal (' '))) [parse_ll::no_skip [fuzz (
                    parse_ll::no_skip [silly_skip])]])];
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            BOOST_CHECK_EQUAL (first (rest (result)), ' ');
        }
        {
            // Nested no_skip without fuzz() in between.
            auto parser = parse_ll::no_skip [fuzz (parse_ll::skip (
                fuzz (parse_ll::literal (' '))) [parse_ll::no_skip [
                    parse_ll::no_skip [silly_skip]]])];
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            BOOST_CHECK_EQUAL (first (rest (result)), ' ');
        }
    }

    {
        std::string r (" a ");
        {
            auto parser = parse_ll::literal ('a');
            auto result = parse (parser, r);
            BOOST_CHECK (!success (result));
        }
        {
            auto parser = parse_ll::skip_pad (parse_ll::literal (' ')) [
                parse_ll::literal ('a')];
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            BOOST_CHECK (empty (rest (result)));
        }
    }
    {
        // Skip should be optional
        std::string r (" a");
        {
            auto parser =
                fuzz (parse_ll::skip_pad (fuzz (parse_ll::literal (' '))) [
                        fuzz (parse_ll::literal ('a'))]);
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            BOOST_CHECK (empty (rest (result)));
        }
    }
    {
        // Skip should be optional
        std::string r ("a ");
        {
            auto parser = parse_ll::skip_pad (parse_ll::literal (' ')) [
                fuzz (parse_ll::literal ('a'))];
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            BOOST_CHECK (empty (rest (result)));
        }
        {
            // no_skip when the main parse already does no skipping.
            auto parser = parse_ll::no_skip [parse_ll::literal ('a')];
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            BOOST_CHECK_EQUAL  (first (rest (result)), ' ');
        }
    }
    {
        std::string r ("   ");
        {
            // Try out silly_skip in the middle.
            auto parser = fuzz (parse_ll::skip_pad (parse_ll::literal (' ')) [
                silly_skip]);
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            BOOST_CHECK (empty (rest (result)));
        }
    }
    {
        std::string r ("  ");
        {
            auto parser = fuzz (parse_ll::skip_pad (parse_ll::literal (' ')) [
                parse_ll::no_skip [silly_skip]]);
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            BOOST_CHECK (empty (rest (result)));
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()
