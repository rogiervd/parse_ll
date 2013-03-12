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
Define a parser that parses an unsigned integer value.
*/

#ifndef PARSE_LL_NUMBER_UNSIGNED_HPP_INCLUDED
#define PARSE_LL_NUMBER_UNSIGNED_HPP_INCLUDED

#include <stdexcept>

#include "utility/returns.hpp"

#include "range/core.hpp"

#include "../core/core.hpp"
#include "../core/repeat.hpp"
#include "../core/named.hpp"
#include "../core/no_skip.hpp"
#include "./digit.hpp"

namespace parse_ll {

/**
Turn a range of values for digits into an unsigned integer.
\todo Different bases?
\todo This could be neatly represented with a fold.

\throw std::overflow_error iff the result does not fit in the integer type.
*/
template <class Result> struct collect_integer {
    template <class DigitValues>
        Result operator() (DigitValues values) const
    {
        // Assume an assignable homogeneous range.
        Result result = Result();
        for (; !::range::empty (values);
            values = ::range::drop (values))
        {
            Result new_result = result * 10 + ::range::first (values);
            if (new_result / 10 != result)
                throw std::overflow_error ("Overflow while parsing integer");
            result = new_result;
        }
        return result;
    }
};

PARSE_LL_DEFINE_NAMED_PARSER_TEMPLATE ("unsigned", unsigned_parser, Result,
    (no_skip [+digit] [collect_integer <Result>()]));

template <typename Result>
    inline auto unsigned_as() RETURNS (unsigned_parser <Result>())
static const auto unsigned_ = unsigned_as <unsigned>();

} // namespace parse_ll

#endif  // PARSE_LL_NUMBER_UNSIGNED_HPP_INCLUDED

