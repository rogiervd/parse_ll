// SPDX-FileCopyrightText: Copyright 2012 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

/** \file
Defines an "end" parser, which only succeeds at the end of the input.
*/

#ifndef PARSE_LL_BASE_END_HPP_INCLUDED
#define PARSE_LL_BASE_END_HPP_INCLUDED

#include "core.hpp"
#include "outcome/explicit.hpp"
#include "outcome/failed.hpp"

#include "range/core.hpp"

namespace parse_ll {

struct end_parser : parser_base<end_parser> {};
static auto const end = end_parser();

struct end_parser_tag;
template <> struct decayed_parser_tag<end_parser>
{
    typedef end_parser_tag type;
};

namespace operation {

    template <> struct parse<end_parser_tag>
    {
        template <class Policy, class Input>
        explicit_outcome<void, Input> operator()(
            Policy const &, end_parser, Input const & input) const
        {
            if (::range::empty(input)) {
                return explicit_outcome<void, Input>(input);
            } else {
                return failed();
            }
        }
    };

    template <> struct describe<end_parser_tag>
    {
        char const * operator()(end_parser const &) const { return "end"; }
    };


}  // namespace operation

}  // namespace parse_ll

#endif  // PARSE_LL_BASE_END_HPP_INCLUDED
