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
        template <class Policy, class SubParser, class ConvertPolicy,
            class Input>
        auto operator() (Policy const & policy,
            change_policy <SubParser, ConvertPolicy> const & directive,
            Input const & input) const
        RETURNS (parse_ll::parse (directive.convert_policy (policy),
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

