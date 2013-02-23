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
Define a sign parser, which parses '-' to -1, and '+' or nothing to +1.
*/

#ifndef PARSE_LL_SIGN_HPP_INCLUDED
#define PARSE_LL_SIGN_HPP_INCLUDED

#include "../core/optional.hpp"
#include "../core/nothing.hpp"
#include "../core/literal.hpp"
#include "../core/alternative.hpp"
#include "../core/transform.hpp"

#include "../core/named.hpp"

#include "boost/phoenix/core/value.hpp"

namespace parse_ll {

PARSE_LL_DEFINE_NAMED_PARSER (sign, (
    literal ('+') [boost::phoenix::val (+1)]
    | literal ('-') [boost::phoenix::val (-1)]
    | nothing [boost::phoenix::val (+1)]));

} // namespace parse_ll

#endif  // PARSE_LL_SIGN_HPP_INCLUDED

