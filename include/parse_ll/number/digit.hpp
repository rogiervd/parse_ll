/*
Copyright 2012 Rogier van Dalen.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
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

