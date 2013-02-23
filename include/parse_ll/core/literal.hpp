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
Define a parser that accepts a literal and does not output anything.
*/

#ifndef PARSE_LL_LITERAL_HPP_INCLUDED
#define PARSE_LL_LITERAL_HPP_INCLUDED

#include <string>

#include "range/core.hpp"

#include "fwd.hpp"
#include "core.hpp"
#include "outcome.hpp"

namespace parse_ll {

/**
Literal must be a range.
It can only be a view if the underlying range is guaranteed to stay in memory
throughout the lifetime of the literal_parser object.
*/
template <class Literal> struct literal_parser
: public parser_base <literal_parser <Literal> >
{
    Literal literal;
public:
    literal_parser (Literal const & literal) : literal (literal) {}
};

inline literal_parser <std::string> literal (char c) {
    return literal_parser <std::string> (std::string (1, c));
}

inline literal_parser <std::string> literal (std::string const & s) {
    return literal_parser <std::string> (s);
}

namespace operation {

    template <class Parse, class Literal, class Input>
        struct parse <Parse, literal_parser <Literal>, Input>
    {
        simple_outcome <void, Input> operator() (Parse const & parse,
            literal_parser <Literal> const & parser, Input input) const
        {
            using ::range::empty; using ::range::first; using ::range::drop;
            auto literal = ::range::view (parser.literal);
            while (!empty (literal) && !empty (input)) {
                if (! (first (literal) == first (input)))
                    return failed();
                literal = drop (literal);
                input = drop (input);
            }
            if (empty (literal))
                // We went through the whole literal: success.
                return simple_outcome <void, Input> (input);
            else
                return failed();
        }
    };

    template <class Literal>
        struct describe <literal_parser <Literal>> {
        const char * operator() (literal_parser <Literal> const &) const
        { return "literal"; }
    };

} // namespace operation

} // namespace parse_ll

#endif  // PARSE_LL_LITERAL_HPP_INCLUDED

