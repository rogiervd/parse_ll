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
Directives usually change the parse.
This defines a base class that does just that.
*/

#ifndef PARSE_LL_BASE_DETAIL_DIRECTIVE_HPP_INCLUDED
#define PARSE_LL_BASE_DETAIL_DIRECTIVE_HPP_INCLUDED

#include "../core.hpp"

namespace parse_ll {

/**
Parser that changes the parse function policy used inside it.
*/
template <class SubParser, class ConvertPolicy> struct change_policy
: parser_base <change_policy <SubParser, ConvertPolicy>> {
    SubParser sub_parser;
    ConvertPolicy convert_policy;
public:
    change_policy (
        SubParser const & sub_parser, ConvertPolicy const & convert_policy)
    : sub_parser (sub_parser), convert_policy (convert_policy) {}
};

struct change_policy_tag;
template <class SubParser, class ConvertPolicy>
    struct decayed_parser_tag <change_policy <SubParser, ConvertPolicy>>
{ typedef change_policy_tag type; };

namespace operation {

    template <> struct parse <change_policy_tag> {
        template <class Parse, class SubParser, class ConvertPolicy,
            class Input>
        auto operator() (Parse const & parse,
            change_policy <SubParser, ConvertPolicy> const & directive,
            Input const & input) const
        RETURNS (parse_with (directive.convert_policy (parse.policy())) (
            directive.sub_parser, input));
    };

    template <> struct describe <change_policy_tag> {
        template <class Parser> const char * operator() (Parser const &) const
        { return "(policy change inside)"; }
    };

} // namespace operation

template <class ConvertPolicy> struct change_policy_directive {
    ConvertPolicy convert_policy;
public:
    explicit change_policy_directive (): convert_policy() {}

    explicit change_policy_directive (ConvertPolicy const & convert_policy)
    : convert_policy (convert_policy) {}

    template <class SubParser>
        change_policy <SubParser, ConvertPolicy>
    operator[] (SubParser const & sub_parser) const {
        return change_policy <SubParser, ConvertPolicy> (
            sub_parser, convert_policy);
    }
};

} // namespace parse_ll

#endif  // PARSE_LL_BASE_DETAIL_DIRECTIVE_HPP_INCLUDED

