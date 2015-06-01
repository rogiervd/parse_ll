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
Define a difference parser, which uses the first parser only if the second one
does not succeed.
*/

#ifndef PARSE_LL_DIFFERENCE_HPP_INCLUDED
#define PARSE_LL_DIFFERENCE_HPP_INCLUDED

#include <boost/optional.hpp>

#include "fwd.hpp"
#include "core.hpp"

namespace parse_ll {

template <class Parser1, class Parser2> struct difference_parser
: parser_base <difference_parser <Parser1, Parser2> >
{
    Parser1 parser_1;
    Parser2 parser_2;
public:
    difference_parser (Parser1 const & parser_1, Parser2 const & parser_2)
    : parser_1 (parser_1), parser_2 (parser_2) {}
};

struct difference_parser_tag;
template <class Parser1, class Parser2>
    struct decayed_parser_tag <difference_parser <Parser1, Parser2>>
{ typedef difference_parser_tag type; };

/**
Outcome for difference_parser.
This tries out parser_2, and if it fails, saves the outcome of parser_1.
In that case, it essentially mimics parser_1.
*/
template <class Policy, class Parser1, class Parser2, class Input>
    struct difference_outcome
{
    typedef typename detail::parser_outcome <Policy, Parser1, Input>::type
        outcome_1_type;
    mutable boost::optional <outcome_1_type> outcome_1;
public:
    difference_outcome (Policy const & policy,
        Parser1 const & parser_1, Parser2 const & parser_2, Input const & input)
    {
        auto outcome_2 = parse (policy, parser_2, input);
        if (!success (outcome_2)) {
            // Try parser_1
            outcome_1 = parse (policy, parser_1, input);
        }
        // Otherwise, leave outcome_1 empty.
    }
};

namespace operation {

    template <> struct parse <difference_parser_tag> {
        template <class Policy, class Parser1, class Parser2, class Input>
            difference_outcome <Policy, Parser1, Parser2, Input> operator() (
                Policy const & policy,
                difference_parser <Parser1, Parser2> const & parser,
                Input const & input) const
        {
            return difference_outcome <Policy, Parser1, Parser2, Input> (
                policy, parser.parser_1, parser.parser_2, input);
        }
    };

    template <> struct describe <difference_parser_tag> {
        template <class Parser> const char * operator() (Parser const &) const
        { return "difference"; }
    };

    template <class Policy, class Parser1, class Parser2, class Input>
        struct success <difference_outcome <Policy, Parser1, Parser2, Input>>
    {
        bool operator() (difference_outcome <Policy, Parser1, Parser2, Input>
                const &outcome) const {
            return outcome.outcome_1 &&
                ::parse_ll::success (*outcome.outcome_1);
        }
    };

    template <class Policy, class Parser1, class Parser2, class Input>
        struct output <difference_outcome <Policy, Parser1, Parser2, Input>>
    {
        auto operator() (difference_outcome <Policy, Parser1, Parser2, Input>
                const & outcome) const
        RETURNS (::parse_ll::output (*outcome.outcome_1));
    };

    template <class Policy, class Parser1, class Parser2, class Input>
        struct rest <difference_outcome <Policy, Parser1, Parser2, Input>>
    {
        Input operator() (difference_outcome <Policy, Parser1, Parser2, Input>
                const & outcome) const {
            return ::parse_ll::rest (*outcome.outcome_1);
        }
    };

} // namespace operation

} // namespace parse_ll

#endif  // PARSE_LL_DIFFERENCE_HPP_INCLUDED

