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
Define a parser wrapper that transforms the output of the parser.
*/

#ifndef PARSE_LL_BASE_TRANSFORM_HPP_INCLUDED
#define PARSE_LL_BASE_TRANSFORM_HPP_INCLUDED

#include "utility/returns.hpp"

#include "fwd.hpp"
#include "core.hpp"

namespace parse_ll {

/*** transform_parser ***/

template <class SubParser, class Actor> struct transform_parser
: public parser_base <transform_parser <SubParser, Actor> >
{
    SubParser sub_parser;
    Actor actor;
public:
    transform_parser (SubParser const & sub_parser, Actor const & actor)
    : sub_parser (sub_parser), actor (actor) {}
};

struct transform_parser_tag;
template <class SubParser, class Actor>
    struct decayed_parser_tag <transform_parser <SubParser, Actor>>
{ typedef transform_parser_tag type; };

template <class Parser, class Actor>
    inline transform_parser <Parser, Actor> transform (
        Parser const & parser, Actor const & actor)
{
    return transform_parser <Parser, Actor> (parser, actor);
}

template <class Policy, class SubParser, class Actor, class Input,
    class SubOutput = typename
        detail::parser_output <Policy, SubParser, Input>::type>
struct transform_outcome {
    typedef typename detail::parser_outcome <Policy, SubParser, Input>::type
        sub_outcome_type;
    sub_outcome_type sub_outcome;
    Actor const * actor;
public:
    transform_outcome (Policy const & policy, SubParser const & sub_parser,
        Input const & input, Actor const & actor)
    : sub_outcome (parse (policy, sub_parser, input)), actor (&actor) {}
};

namespace operation {

    template <> struct parse <transform_parser_tag> {
        template <class Policy, class SubParser, class Actor, class Input>
            auto operator() (Policy const & policy,
                transform_parser <SubParser, Actor> const & parser,
                Input const & input) const
        RETURNS (transform_outcome <Policy, SubParser, Actor, Input> (
            policy, parser.sub_parser, input, parser.actor));
    };

    template <> struct describe <transform_parser_tag> {
        template <class Parser> const char * operator() (Parser const &) const
        { return "transform"; }
    };

    template <class Policy, class SubParser, class Actor, class Input>
        struct success <transform_outcome <Policy, SubParser, Actor, Input>> {
        bool operator() (transform_outcome <Policy, SubParser, Actor, Input>
            const & outcome) const
        { return ::parse_ll::success (outcome.sub_outcome); }
    };

    template <class Policy, class SubParser, class Actor, class Input,
            typename SubOutput>
        struct output <transform_outcome <
            Policy, SubParser, Actor, Input, SubOutput>> {
        auto operator() (transform_outcome <Policy, SubParser, Actor, Input>
            const & outcome) const
        RETURNS ((*outcome.actor) (::parse_ll::output (outcome.sub_outcome)));
    };
    template <class Policy, class SubParser, class Actor, class Input>
        struct output <
            transform_outcome <Policy, SubParser, Actor, Input, void>>
    {
        auto operator() (transform_outcome <Policy, SubParser, Actor, Input>
            const & outcome) const
        RETURNS ((*outcome.actor)());
    };

    template <class Policy, class SubParser, class Actor, class Input>
        struct rest <transform_outcome <Policy, SubParser, Actor, Input>> {
        auto operator() (transform_outcome <Policy, SubParser, Actor, Input>
            const & outcome) const
        RETURNS (::parse_ll::rest (outcome.sub_outcome));
    };

} // namespace operation

} // namespace parse_ll

#endif  // PARSE_LL_BASE_TRANSFORM_HPP_INCLUDED

