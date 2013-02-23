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
Define an alternative parser, which tries to parse the same input with different
parsers until one of them succeeds.
*/

#ifndef PARSE_LL_ALTERNATIVE_HPP_INCLUDED
#define PARSE_LL_ALTERNATIVE_HPP_INCLUDED

#include "utility/returns.hpp"

#include "fwd.hpp"
#include "core.hpp"

#include "outcome.hpp"

#include <boost/variant.hpp>

namespace parse_ll {

template <class Parser1, class Parser2> struct alternative_parser
: parser_base <alternative_parser <Parser1, Parser2> >
{
    Parser1 parser_1;
    Parser2 parser_2;
public:
    alternative_parser (Parser1 const & parser_1, Parser2 const & parser_2)
    : parser_1 (parser_1), parser_2 (parser_2) {}
};

template <class... Parsers> struct make_alternative;

/**
alternative (p1, p2, p3) is equivalent to p1 | p2 | p3.

The number of parsers must be at least 1.

This uses make_alternative because GCC 4.6 didn't like the one with functions.
*/
template <class ... Parsers> inline
    typename make_alternative <Parsers...>::result_type
    alternative (Parsers const & ... parsers)
{
    make_alternative <Parsers...> implementation;
    return implementation (parsers...);
}

template <class Parser> struct make_alternative <Parser> {
    typedef Parser result_type;
    Parser operator() (Parser const & parser) const { return parser; }
};

template <class Parser1, class Parser2, class... Parsers>
struct make_alternative <Parser1, Parser2, Parsers...> {
    typedef alternative_parser <Parser1, Parser2> first_two;
    typedef make_alternative <first_two, Parsers...> sub_make;
    typedef typename sub_make::result_type result_type;

    result_type operator() (Parser1 const & parser_1, Parser2 const & parser_2,
        Parsers const & ... parsers) const
    {
        sub_make sub;
        return sub (first_two (parser_1, parser_2), parsers...);
    }
};

// This fine implementation does not work in GCC 4.6: it doesn't like the
// recursive case calling itself while figuring out the return type, it seems?
/*
// Base case.
template <class Parser1> inline
    Parser1 alternative (Parser1 const & parser_1)
{ return parser_1; }

// Recursive case.
template <class Parser1, class Parser2, class ... Parsers> inline
    auto alternative (Parser1 const & parser_1,
        Parser2 const &parser_2, Parsers const & ... parsers)
RETURNS (alternative (
    alternative_parser <Parser1, Parser2> (parser_1, parser_2), parsers...))
*/

namespace operation {

    template <class Parse, class Parser1, class Parser2, class Input>
        struct parse <Parse, alternative_parser <Parser1, Parser2>, Input>
    {
        typedef typename detail::parser_outcome <Parse, Parser1, Input>::type
            outcome_1_type;
        typedef typename detail::parser_outcome <Parse, Parser2, Input>::type
            outcome_2_type;
        typedef typename detail::outcome_output <outcome_1_type>::type
            output_1_type;
        typedef typename detail::outcome_output <outcome_2_type>::type
            output_2_type;

        typedef decltype (true ?
            ::parse_ll::output (std::declval <outcome_1_type>()) :
            ::parse_ll::output (std::declval <outcome_2_type>())) output_type;
        typedef simple_outcome <output_type, Input> outcome_type;

        outcome_type operator() (Parse const & parse,
                alternative_parser <Parser1, Parser2> const & parser,
                Input const & input) const
        {
            {
                // Try parser 1
                outcome_1_type outcome_1 = parse (parser.parser_1, input);
                if (::parse_ll::success (outcome_1))
                    return outcome_type (std::move (outcome_1));
                // Failed; destruct outcome_1.
            }
            // Try parser 2
            outcome_2_type outcome_2 = parse (parser.parser_2, input);
            if (::parse_ll::success (outcome_2))
                return outcome_type (std::move (outcome_2));
            // Otherwise, fail.
            return failed();
        }
    };

    template <class Parser1, class Parser2>
        struct describe <alternative_parser <Parser1, Parser2>> {
        const char * operator() (alternative_parser <Parser1, Parser2> const &)
            const
        { return "alternative"; }
    };

} // namespace operation

} // namespace parse_ll

#endif  // PARSE_LL_ALTERNATIVE_HPP_INCLUDED

