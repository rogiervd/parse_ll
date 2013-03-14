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
Defines an "end" parser, which only succeeds at the end of the input.
*/

#ifndef PARSE_LL_BASE_END_HPP_INCLUDED
#define PARSE_LL_BASE_END_HPP_INCLUDED

#include "core.hpp"
#include "outcome/explicit.hpp"
#include "outcome/failed.hpp"

#include "range/core.hpp"

namespace parse_ll {

struct end_parser : parser_base <end_parser> {};
static const auto end = end_parser();

namespace operation {

    template <class Parse, class Input> struct parse <Parse, end_parser, Input>
    {
        explicit_outcome <void, Input>
            operator() (Parse const &, end_parser, Input const & input) const {
            if (::range::empty (input))
                return explicit_outcome <void, Input> (input);
            else
                return failed();
        }
    };

    template <> struct describe <end_parser> {
        const char * operator() (end_parser const &) const
        { return "end"; }
    };


} // namespace operation

} // namespace parse_ll

#endif  // PARSE_LL_BASE_END_HPP_INCLUDED

