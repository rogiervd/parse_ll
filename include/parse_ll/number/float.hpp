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
Define a floating-point parser.
*/

#ifndef PARSE_LL_NUMBER_FLOAT_HPP_INCLUDED
#define PARSE_LL_NUMBER_FLOAT_HPP_INCLUDED

#include "utility/returns.hpp"

#include "range/walk_size.hpp"

#include "../core/fwd.hpp"
#include "../core/core.hpp"
#include "../core/sequence.hpp"
#include "../core/char.hpp"
#include "../core/literal.hpp"
#include "../core/nothing.hpp"
#include "../core/no_skip.hpp"

#include "sign.hpp"
#include "digit.hpp"
#include "int.hpp"

namespace parse_ll {

template <typename Result> struct convert_float {
    Result operator() (std::tuple <int, std::tuple <Result, int>, int>
        const & data) const
    {
        int sign, exponent; std::tuple <Result, int> mantissa_;
        std::tie (sign, mantissa_, exponent) = data;
        Result mantissa; int offset;
        std::tie (mantissa, offset) = mantissa_;

        using std::pow;
        return sign * mantissa * pow (Result (10), exponent - offset);
    }
};

template <typename Result> class convert_mantissa {
    template <class Digits>
    Result add_digits (Result current, Digits digits) const {
        for (; !::range::empty (digits); digits = ::range::drop (digits))
            current = current * 10 + ::range::first (digits);
        return current;
    }

public:
    template <class Digits>
    std::tuple <Result, int> operator() (
        std::tuple <Digits, boost::optional <
            std::tuple <Digits> > const &> const & data) const
    {
        Digits before_dot = std::get <0> (data);
        Result current = add_digits (Result(), before_dot);
        if (std::get <1> (data)) {
            Digits after_dot = std::get <0> (std::get <1> (data).get());
            current = add_digits (current, after_dot);
            return std::tuple <Result, int> (
                current, range::walk_size (after_dot));
        } else
            return std::tuple <Result, int> (current, 0);
    }

    template <class Digits>
    std::tuple <Result, int> operator() (
        std::tuple <Digits> const & data) const
    {
        Digits after_dot = std::get <0> (data);
        Result current = add_digits (Result(), after_dot);
        return std::tuple <Result, int> (current, range::walk_size (after_dot));
    }
};

struct extract_exponent {
    template <typename Int>
    int operator() (boost::optional <std::tuple <Int> > const & data)
        const
    {
        if (!data)
            return 0;
        else
            return std::get <0> (data.get());
    }
};

PARSE_LL_DEFINE_NAMED_PARSER_TEMPLATE ("float_exponent",
    float_exponent_parser, Result,
    no_skip [-(literal('e') >> int_as <Result>())] [extract_exponent()]);

template <typename Result>
    auto float_exponent_as() RETURNS (float_exponent_parser <Result>())

static const auto float_exponent = float_exponent_as <int> ();

template <class Result> struct tuple_and_int {
    typedef std::tuple <Result, int> type;
};

PARSE_LL_DEFINE_NAMED_PARSER_TEMPLATE ("mantissa", mantissa_parser, Result,
    no_skip [
        (+digit >> -(literal ('.') >> *digit)) [convert_mantissa <Result>()]
        | (literal ('.') >> +digit) [convert_mantissa <Result>()]]
    );

PARSE_LL_DEFINE_NAMED_PARSER_TEMPLATE ("float", float_parser, Result,
    no_skip [sign >> mantissa_parser <Result>() >> float_exponent]
        [convert_float <Result>()]);

template <class Result>
    inline auto float_as() RETURNS (float_parser <Result> ())

static const auto float_ = float_as <double>();

} // namespace parse_ll

#endif  // PARSE_LL_NUMBER_FLOAT_HPP_INCLUDED

