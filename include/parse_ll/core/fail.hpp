// SPDX-FileCopyrightText: Copyright 2013 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

/** \file
Define a parser that always fails.
*/

#ifndef PARSE_LL_FAIL_HPP_INCLUDED
#define PARSE_LL_FAIL_HPP_INCLUDED

#include "utility/returns.hpp"

#include "detail/parser_base.hpp"
#include "fwd.hpp"
// Do not include core.hpp: this file is included in core.hpp.
#include "outcome/failed.hpp"

namespace parse_ll {

// Parser that always fails.
struct fail_parser : parser_base<fail_parser> {};
static auto const fail = fail_parser();

struct fail_parser_tag;

template <> struct decayed_parser_tag<fail_parser>
{
    typedef fail_parser_tag type;
};

namespace operation {

    template <> struct parse<fail_parser_tag>
    {
        template <class Policy, class Input>
        auto operator()(Policy const &, fail_parser const &, Input &&) const
            RETURNS(failed());
    };

    template <> struct skip_over<fail_parser_tag>
    {
        template <class Policy, class Input> Input && operator()(
            Policy const &, fail_parser const &, Input && input) const
        {
            return std::forward<Input>(input);
        }
    };

    template <> struct describe<fail_parser_tag>
    {
        char const * operator()(fail_parser const &) const { return "fail"; }
    };

}  // namespace operation

}  // namespace parse_ll

#endif  // PARSE_LL_FAIL_HPP_INCLUDED
