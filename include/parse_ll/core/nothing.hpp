// SPDX-FileCopyrightText: Copyright 2012 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

/** \file
Define a parser that parses nothing of the input and always succeeds.
*/

#ifndef PARSE_LL_NOTHING_HPP_INCLUDED
#define PARSE_LL_NOTHING_HPP_INCLUDED

#include "utility/returns.hpp"

#include "core.hpp"
#include "outcome/successful.hpp"

namespace parse_ll {

// Parser that does not parse any input but is always successful.
struct nothing_parser : parser_base<nothing_parser> {};
static auto const nothing = nothing_parser();

struct nothing_parser_tag;

template <> struct decayed_parser_tag<nothing_parser>
{
    typedef nothing_parser_tag type;
};

namespace operation {

    template <> struct parse<nothing_parser_tag>
    {
        template <class Parse, class Input> auto operator()(
            Parse const &, nothing_parser const &, Input && input) const
            RETURNS(
                successful<void, typename std::decay<Input>::type>(
                    std::forward<Input>(input)));
    };

    template <> struct describe<nothing_parser_tag>
    {
        char const * operator()(nothing_parser const &) const
        {
            return "nothing";
        }
    };
}  // namespace operation

}  // namespace parse_ll

#endif  // PARSE_LL_NOTHING_HPP_INCLUDED
