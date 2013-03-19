/*
Copyright 2013 Rogier van Dalen.

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
Define a parser that always fails.
*/

#ifndef PARSE_LL_FAIL_HPP_INCLUDED
#define PARSE_LL_FAIL_HPP_INCLUDED

#include "utility/returns.hpp"

#include "fwd.hpp"
#include "detail/parser_base.hpp"
// Do not include core.hpp: this file is included in core.hpp.
#include "outcome/failed.hpp"

namespace parse_ll {

// Parser that always fails.
struct fail_parser : parser_base <fail_parser> {};
static const auto fail = fail_parser();

struct fail_parser_tag;

template <> struct decayed_parser_tag <fail_parser>
{ typedef fail_parser_tag type; };

namespace operation {

    template <> struct parse <fail_parser_tag> {
        template <class Policy, class Input> auto operator() (
            Policy const &, fail_parser const &, Input &&) const
        RETURNS (failed())
    };

    template <> struct skip_over <fail_parser_tag> {
        template <class Policy, class Input> Input && operator() (
            Policy const &, fail_parser const &, Input && input) const
        { return std::forward <Input> (input); }
    };

    template <> struct describe <fail_parser_tag> {
        const char * operator() (fail_parser const &) const { return "fail"; }
    };

} // namespace operation

} // namespace parse_ll

#endif // PARSE_LL_FAIL_HPP_INCLUDED

