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
Define a polymorphic parser.
*/

#ifndef PARSE_LL_RULE_HPP_INCLUDED_HPP
#define PARSE_LL_RULE_HPP_INCLUDED_HPP

#include <memory>
#include <type_traits>

#include "core.hpp"
#include "outcome.hpp"

namespace parse_ll {

template <class Input, class Output> struct rule;

namespace detail {

    template <class Input, class Output> class polymorphic_parser
    {
    public:
        virtual ~polymorphic_parser() {}

        typedef explicit_outcome <Output, Input> outcome_type;

        virtual outcome_type parse_from (Input const & input) const
            = 0;
    };

    template <class Input, class Output, class Parser>
        class polymorphic_parser_implementation
    : public polymorphic_parser <Input, Output>
    {
        Parser parser;
    public:
        polymorphic_parser_implementation (Parser const & parser)
        : parser (parser) {}

        virtual ~polymorphic_parser_implementation() {}

        typedef typename polymorphic_parser <Input, Output>::outcome_type
            outcome_type;

        virtual outcome_type parse_from (Input const & input) const {
            // Use ::parse_ll::parse: the parse that the rule is called with is
            // ignored.
            auto outcome = ::parse_ll::parse (parser, input);
            return outcome_type (outcome);
        }
    };

} // namespace detail

/**
A parser with an implementation opaque to the caller and not visible in its
type.
It only needs to know the input type (a range) and the output type.
It can be initialised with any parser that can take an input range of type Input
and whose output can be converted to type Output.
It can also be assigned to such a parser.
This way, rules can be used as parsers without requiring their innards to be
exposed as well.

Rules ignore properties of the parse, such as skip parsers, they are called
with.

\internal
This class works by keeping a pointer to an object of a virtual base class,
detail::polymorphic_parser.
The implementation, of the non-abstract derived class
detail::polymorphic_parser_implementation is only known at the time of
instantiation.
Therefore, the rule can be assigned a parser that is instantiated only in that
compilation unit, but used in other compilation units.
*/
template <class Input, class Output> struct rule
    : public parser_base <rule <Input, Output> >
{
    // This must be a shared_ptr or this class cannot be copied.
    std::shared_ptr <detail::polymorphic_parser <Input, Output> >
        implementation;
public:
    rule() {}

    template <class Parser> rule (Parser const & parser)
    : implementation (new detail::polymorphic_parser_implementation <
        Input, Output, Parser> (parser)) {}
};

struct rule_tag;
template <class Input, class Output>
    struct decayed_parser_tag <rule <Input, Output>>
{ typedef rule_tag type; };

namespace operation {

    template <> struct parse <rule_tag> {
        template <class Parse, class RuleInput, class Output, class ActualInput>
        // Always generate a return type, to prevent strange errors.
            explicit_outcome <Output, ActualInput> operator() (
                Parse const &, rule <RuleInput, Output> const & parser,
                ActualInput const & input) const
        {
            // Generate an error here.
            static_assert (std::is_same <RuleInput, ActualInput>::value,
                "The rule parser can only parse with a fixed Input type");
            // Ignore Parse.
            return parser.implementation->parse_from (input);
        }
    };

    template <> struct describe <rule_tag> {
        template <class Rule> const char * operator() (Rule const &) const
        { return "rule (opaque)"; }
    };

} // namespace operation

} // namespace parse_ll

#endif  // PARSE_LL_RULE_HPP_INCLUDED_HPP

