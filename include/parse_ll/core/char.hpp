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
Define a parser that parses one character.
*/

#ifndef PARSE_LL_CHAR_HPP_INCLUDED
#define PARSE_LL_CHAR_HPP_INCLUDED

#include <boost/optional.hpp>

#include "range/core.hpp"

#include "fwd.hpp"
#include "core.hpp"
#include "outcome/failed.hpp"

namespace parse_ll {

/*** char_parser ***/
template <class Match> struct char_parser
: public parser_base <char_parser <Match> >
{
    Match match;
public:
    char_parser() {}
    char_parser (Match const & match) : match (match) {}
};

struct char_parser_tag;

template <class Match> struct decayed_parser_tag <char_parser <Match>>
{ typedef char_parser_tag type; };

/**
Outcome that, if successful, returns the first element of the input as output
and the rest of the input as rest.
*/
template <class Input> struct char_outcome {
    boost::optional <Input> successful_input;
public:
    char_outcome (failed) {}
    char_outcome (Input const & input) : successful_input (input) {}
};

namespace operation {

    template <> struct parse <char_parser_tag> {
        template <class Policy, class Match, class Input>
            char_outcome <Input> operator() (Policy const &,
                char_parser <Match> const & parser, Input const & input) const
        {
            if (!::range::empty (input)
                && parser.match (::range::first (input)))
            {
                return char_outcome <Input> (input);
            } else
                return failed();
        }
    };

    template <> struct describe <char_parser_tag> {
        template <class Parser> const char * operator() (Parser const &) const
        { return "character"; }
    };

    template <class Input> struct success <char_outcome <Input>> {
        bool operator() (char_outcome <Input> const & outcome) const {
            return bool (outcome.successful_input);
        }
    };

    template <class Input> struct output <char_outcome <Input>> {
        auto operator() (char_outcome <Input> const & outcome) const
            RETURNS (::range::first (outcome.successful_input.get()));
    };

    template <class Input> struct rest <char_outcome <Input>> {
        Input operator() (char_outcome <Input> const & outcome) const {
            return ::range::drop (outcome.successful_input.get());
        }
    };

} // namespace operation

struct match_any {
    template <class Char> bool operator() (Char const &) const { return true; }
};

template <class Char>
struct match_one {
    Char expected;
    match_one (Char const & expected) : expected (expected) {}
    bool operator() (Char const & found) const { return found == expected; }
};

/**
char_parser that matches any character.
It also has operator() to turn it into a parser that matches only a specific
character.
This is a trick that allows both
    char_
and
    char_('a')
as parsers.
*/
struct any_char_parser : char_parser <match_any>
{
    any_char_parser() {}

    template <class Char>
        char_parser <match_one <Char> > operator() (Char const & expected) const
    {
        return char_parser <match_one <Char> > (expected);
    }
};

template <> struct decayed_parser_tag <any_char_parser>
{ typedef char_parser_tag type; };

static const auto char_ = any_char_parser();

} // namespace parse_ll

#endif  // PARSE_LL_CHAR_HPP_INCLUDED

