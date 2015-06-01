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

