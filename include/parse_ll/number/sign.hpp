// SPDX-FileCopyrightText: Copyright 2012 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

/** \file
Define a sign parser, which parses '-' to -1, and '+' or nothing to +1.
*/

#ifndef PARSE_LL_SIGN_HPP_INCLUDED
#define PARSE_LL_SIGN_HPP_INCLUDED

#include "../core/alternative.hpp"
#include "../core/literal.hpp"
#include "../core/nothing.hpp"
#include "../core/optional.hpp"
#include "../core/transform.hpp"

#include "../core/named.hpp"

#include "boost/phoenix/core/value.hpp"

namespace parse_ll {

PARSE_LL_DEFINE_NAMED_PARSER(
    sign,
    (literal('+')[boost::phoenix::val(+1)]
     | literal('-')[boost::phoenix::val(-1)]
     | nothing[boost::phoenix::val(+1)]));

}  // namespace parse_ll

#endif  // PARSE_LL_SIGN_HPP_INCLUDED
