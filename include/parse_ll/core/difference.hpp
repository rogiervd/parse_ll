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
Define a difference parser, which uses the first parser only if the second one
does not succeed.
*/

#ifndef PARSE_LL_DIFFERENCE_HPP_INCLUDED
#define PARSE_LL_DIFFERENCE_HPP_INCLUDED

#include <boost/optional.hpp>

#include "fwd.hpp"
#include "core.hpp"

namespace parse_ll {

template <class Parser1, class Parser2> struct difference_parser
: parser_base <difference_parser <Parser1, Parser2> >
{
    Parser1 parser_1;
    Parser2 parser_2;
public:
    difference_parser (Parser1 const & parser_1, Parser2 const & parser_2)
    : parser_1 (parser_1), parser_2 (parser_2) {}
};

struct difference_parser_tag;
template <class Parser1, class Parser2>
    struct decayed_parser_tag <difference_parser <Parser1, Parser2>>
{ typedef difference_parser_tag type; };

/**
Outcome for difference_parser.
This tries out parser_2, and if it fails, saves the outcome of parser_1.
In that case, it essentially mimics parser_1.
*/
template <class Parse, class Parser1, class Parser2, class Input>
    struct difference_outcome
{
    typedef typename detail::parser_outcome <Parse, Parser1, Input>::type
        outcome_1_type;
    mutable boost::optional <outcome_1_type> outcome_1;
public:
    difference_outcome (Parse const & parse,
        Parser1 const & parser_1, Parser2 const & parser_2, Input const & input)
    {
        auto outcome_2 = parse (parser_2, input);
        if (!success (outcome_2)) {
            // Try parser_1
            outcome_1 = parse (parser_1, input);
        }
        // Otherwise, leave outcome_1 empty.
    }
};

namespace operation {

    template <> struct parse <difference_parser_tag> {
        template <class Parse, class Parser1, class Parser2, class Input>
            difference_outcome <Parse, Parser1, Parser2, Input> operator() (
                Parse const & parse,
                difference_parser <Parser1, Parser2> const & parser,
                Input const & input) const
        {
            return difference_outcome <Parse, Parser1, Parser2, Input> (parse,
                parser.parser_1, parser.parser_2, input);
        }
    };

    template <> struct describe <difference_parser_tag> {
        template <class Parser> const char * operator() (Parser const &) const
        { return "difference"; }
    };

    template <class Parse, class Parser1, class Parser2, class Input>
        struct success <difference_outcome <Parse, Parser1, Parser2, Input>>
    {
        bool operator() (difference_outcome <Parse, Parser1, Parser2, Input>
                const &outcome) const {
            return outcome.outcome_1 &&
                ::parse_ll::success (*outcome.outcome_1);
        }
    };

    template <class Parse, class Parser1, class Parser2, class Input>
        struct output <difference_outcome <Parse, Parser1, Parser2, Input>>
    {
        auto operator() (difference_outcome <Parse, Parser1, Parser2, Input>
                const & outcome) const
        RETURNS (::parse_ll::output (*outcome.outcome_1));
    };

    template <class Parse, class Parser1, class Parser2, class Input>
        struct rest <difference_outcome <Parse, Parser1, Parser2, Input>>
    {
        Input operator() (difference_outcome <Parse, Parser1, Parser2, Input>
                const & outcome) const {
            return ::parse_ll::rest (*outcome.outcome_1);
        }
    };

} // namespace operation

} // namespace parse_ll

#endif  // PARSE_LL_DIFFERENCE_HPP_INCLUDED

