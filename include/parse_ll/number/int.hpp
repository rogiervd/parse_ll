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

#ifndef PARSE_LL_NUMBER_INT_HPP_INCLUDED
#define PARSE_LL_NUMBER_INT_HPP_INCLUDED

#include <utility>

#include "utility/returns.hpp"

#include "sign.hpp"
#include "unsigned.hpp"
#include "../core/core.hpp"
#include "../core/sequence.hpp"
#include "../core/nothing.hpp"

namespace parse_ll {

/**
Turn a range of values for digits into a signed integer.

\throw std::overflow_error iff the result does not fit in the integer type.
*/
template <class Result> struct combine_sign {
    Result operator() (std::tuple <int, Result const &> const & data) const
    {
        int sign;
        Result absolute;
        std::tie (sign, absolute) = data;
        return Result (sign * absolute);
    }
};

PARSE_LL_DEFINE_NAMED_PARSER_TEMPLATE ("int", int_parser, Result,
    (no_skip [sign >> unsigned_as <Result>()] [combine_sign <Result>()]));

template <typename Result>
    inline auto int_as() RETURNS (int_parser <Result>());
static const auto int_ = int_as <int>();

} // namespace parse_ll

#endif  // PARSE_LL_NUMBER_INT_HPP_INCLUDED

