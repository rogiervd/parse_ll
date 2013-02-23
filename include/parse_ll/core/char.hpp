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
Define a parser that parses one character.
*/

#ifndef PARSE_LL_CHAR_HPP_INCLUDED
#define PARSE_LL_CHAR_HPP_INCLUDED

#include "range/core.hpp"

#include "fwd.hpp"
#include "core.hpp"
#include "outcome.hpp"

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

    template <class Parse, class Match, class Input>
        struct parse <Parse, char_parser <Match>, Input>
    {
        char_outcome <Input> operator() (Parse const & parse,
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

    template <class Match> struct describe <char_parser <Match>> {
        const char * operator() (char_parser <Match> const &) const
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


static const auto char_ = any_char_parser();

} // namespace parse_ll

#endif  // PARSE_LL_CHAR_HPP_INCLUDED

