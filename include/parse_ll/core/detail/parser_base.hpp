/*
Copyright 2012, 2013 Rogier van Dalen.

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
Base classes for parser.
*/

#ifndef PARSE_LL_CORE_DETAIL_PARSER_BASE_HPP_INCLUDED
#define PARSE_LL_CORE_DETAIL_PARSER_BASE_HPP_INCLUDED

#include "range/core.hpp"

#include "../fwd.hpp"

namespace parse_ll {

/*** parser ***/

template <class Derived> struct parser_base {
    Derived const * this_() const
    { return static_cast <Derived const *> (this); }

    repeat_parser <Derived> operator* () const {
        return repeat_parser <Derived> (*this_(), 0, -1);
    }
    repeat_parser <Derived> operator+ () const {
        return repeat_parser <Derived> (*this_(), 1, -1);
    }

    optional_parser <Derived> operator- () const {
        return optional_parser <Derived> (*this_());
    }

    template <class OtherParser>
        alternative_parser <Derived, OtherParser>
            operator| (OtherParser const & other) const
    {
        return alternative_parser <Derived, OtherParser> (*this_(), other);
    }

    template <class OtherParser>
        sequence_parser <false, Derived, OtherParser>
            operator >> (OtherParser const & other) const
    {
        return sequence_parser <false, Derived, OtherParser> (*this_(), other);
    }

    /**
    \todo Make sure there's no difference between
        a > b >> c and (a > b) >> c.
    */
    template <class OtherParser>
        sequence_parser <true, Derived, OtherParser>
            operator > (OtherParser const & other) const
    {
        return sequence_parser <true, Derived, OtherParser> (*this_(), other);
    }

    template <class OtherParser>
        difference_parser <Derived, OtherParser>
            operator - (OtherParser const & other) const
    {
        return difference_parser <Derived, OtherParser> (*this_(), other);
    }

    template <class Actor>
    transform_parser <Derived, Actor> operator[] (Actor const & actor) const {
        return transform_parser <Derived, Actor> (*this_(), actor);
    }
};

} // namespace parse_ll

#endif // PARSE_LL_CORE_DETAIL_PARSER_BASE_HPP_INCLUDED

