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
Define a parser that parses one digit.
*/

#ifndef PARSE_LL_DIGIT_HPP_INCLUDED
#define PARSE_LL_DIGIT_HPP_INCLUDED

#include "../core/char.hpp"
#include "../core/transform.hpp"

namespace parse_ll {

/**
Digit matcher for char_parser.
This matches decimal digits.
It may be interesting to consider how to generalise this (and int_, and float_)
to other bases.
*/
struct match_digit {
    template <class Char>
    bool operator() (Char const & c) const {
        return '0' <= c && c <= '9';
    }
};

struct convert_digit {
    template <class Char>
    int operator() (Char const & c) const {
        return int (c - '0');
    }
};

static const auto digit = char_parser <match_digit>() [convert_digit()];

} // namespace parse_ll

#endif  // PARSE_LL_DIGIT_HPP_INCLUDED

