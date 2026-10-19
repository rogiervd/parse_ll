// SPDX-FileCopyrightText: Copyright 2012 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

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
struct match_digit
{
    template <class Char> bool operator()(Char const & c) const
    {
        return '0' <= c && c <= '9';
    }
};

struct convert_digit
{
    template <class Char> int operator()(Char const & c) const
    {
        return int(c - '0');
    }
};

static auto const digit = char_parser<match_digit>()[convert_digit()];

}  // namespace parse_ll

#endif  // PARSE_LL_DIGIT_HPP_INCLUDED
