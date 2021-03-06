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

