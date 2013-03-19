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
Define a parser that parses nothing of the input and always succeeds.
*/

#ifndef PARSE_LL_NOTHING_HPP_INCLUDED
#define PARSE_LL_NOTHING_HPP_INCLUDED

#include "utility/returns.hpp"

#include "core.hpp"
#include "outcome/successful.hpp"

namespace parse_ll {

// Parser that does not parse any input but is always successful.
struct nothing_parser : parser_base <nothing_parser> {};
static const auto nothing = nothing_parser();

struct nothing_parser_tag;

template <> struct decayed_parser_tag <nothing_parser>
{ typedef nothing_parser_tag type; };

namespace operation {

    template <> struct parse <nothing_parser_tag> {
        template <class Parse, class Input> auto operator() (
            Parse const &, nothing_parser const &, Input && input) const
        RETURNS (successful <void, typename std::decay <Input>::type> (
            std::forward <Input> (input)))
    };

    template <> struct describe <nothing_parser_tag> {
        const char * operator() (nothing_parser const &) const
        { return "nothing"; }
    };
} // namespace operation

} // namespace parse_ll

#endif  // PARSE_LL_NOTHING_HPP_INCLUDED

