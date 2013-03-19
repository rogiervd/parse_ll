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

#include "outcome.hpp"
#include "core.hpp"
#include "fail.hpp"

namespace parse_ll {

/**
A parser with an implementation opaque to the caller and not visible in its
type.
It only needs to know the input type (a range) and the output type.
It can be initialised with any parser that can take an input range of type Input
and whose output can be converted to type Output.
It can also be assigned to such a parser.
This way, rules can be used as parsers without requiring their innards to be
exposed as well.

If no SkipParser is given, the skip parser outside of the rule is propagated
internally.
This uses a virtual function call every time the skip parser is required.
Alternatively, the correct SkipParser can be specified.
It must then have exactly the same type as in the policy that the rule is used
with.
Alternatively, SkipParser can be set to rule_explicit_skip_parser.
In that case, the rule inside must provide a skip parser for itself, for example
with skip (..) [...] or no_skip [], otherwise a compile error is generated.

\internal
This class works by keeping a pointer to an object of a virtual base class,
detail::polymorphic_parser.
The implementation, of the non-abstract derived class
detail::polymorphic_parser_implementation is only known at the time of
instantiation.
Therefore, the rule can be assigned a parser that is instantiated only in that
compilation unit, but used in other compilation units.
*/
template <class Input, class Output, class SkipParser> struct rule;

class rule_explicit_skip_parser;

namespace detail {

    /**
    \return An appropriate proxy type for the skip parser.
    If SkipParser is non-void, this is just the skip parser itself.
    If SkipParser is void, this is a rule.
    */
    template <class Input, class SkipParser> struct skip_parser_proxy
    { typedef SkipParser type; };

    template <class Input> struct skip_parser_proxy <Input, void>
    { typedef rule <Input, void, fail_parser> type; };

    /**
    Polymorphic policy.
    If SkipParser is non-void, this will have copied the skip parser from the
    original policy, and no actual polymorphism is needed.
    */
    template <class Input, class SkipParser> class polymorphic_policy
    : public parse_policy::direct
    {
        typedef typename skip_parser_proxy <Input, SkipParser>::type
            skip_parser_type;
        skip_parser_type skip_parser_;
    protected:
        // No copying unless the derived class is happy with it.
        polymorphic_policy (polymorphic_policy const &) = default;
    public:
        template <class OriginalPolicy>
            polymorphic_policy (OriginalPolicy const & original_policy)
        : skip_parser_ (original_policy.skip_parser()) {}

        skip_parser_type const & skip_parser() const { return skip_parser_; }
    };

    /**
    Implementation of the polymorphic policy.
    At the moment, this does not add anything to polymorphic_policy.
    */
    template <class Input, class SkipParser>
        class polymorphic_policy_implementation
    : public polymorphic_policy <Input, SkipParser>
    {
        typedef polymorphic_policy <Input, SkipParser> base;
    public:
        template <class OriginalPolicy> polymorphic_policy_implementation (
            OriginalPolicy const & original_policy)
        : base (original_policy) {}
    };

    /**
    A policy with an implementation opaque to the caller.
    This class is copyable.
    */
    template <class Input, class SkipParser> class opaque_policy {
        typedef polymorphic_policy <Input, SkipParser> actual_policy_type;
        std::shared_ptr <actual_policy_type> actual_policy;

        typedef typename skip_parser_proxy <Input, SkipParser>::type
            skip_parser_type;
    public:
        template <class OriginalPolicy>
            opaque_policy (OriginalPolicy const & original_policy)
        : actual_policy (
            std::make_shared <actual_policy_type> (original_policy)) {}

        template <class Apply, class Policy, class Parser, class ActualInput>
            auto apply_parse (Policy const & policy, Parser const & parser,
                ActualInput && input) const
        RETURNS (actual_policy->template apply_parse <Apply> (
            policy, parser, std::forward <ActualInput> (input)))

        skip_parser_type const & skip_parser() const
        { return actual_policy->skip_parser(); }
    };

    /**
    Polymorphic parser.
    */
    template <class Input, class Output, class SkipParser>
        class polymorphic_parser
    {
    public:
        virtual ~polymorphic_parser() {}

        typedef explicit_outcome <Output, Input> outcome_type;

        virtual outcome_type parse_from (
            opaque_policy <Input, SkipParser> const & policy,
            Input const & input) const = 0;
    };

    template <class Input, class Output, class SkipParser, class Parser>
        class polymorphic_parser_implementation
    : public polymorphic_parser <Input, Output, SkipParser>
    {
        Parser parser;
    public:
        polymorphic_parser_implementation (Parser const & parser)
        : parser (parser) {}

        virtual ~polymorphic_parser_implementation() {}

        typedef typename polymorphic_parser <Input, Output, SkipParser
            >::outcome_type outcome_type;

        virtual outcome_type parse_from (
            opaque_policy <Input, SkipParser> const & policy,
            Input const & input) const
        {
            auto outcome = ::parse_ll::parse (policy, parser, input);
            return outcome_type (std::move (outcome));
        }
    };

} // namespace detail

template <class Input, class Output, class SkipParser> struct rule
    : public parser_base <rule <Input, Output> >
{
    // This must be a shared_ptr or this class cannot be copied.
    std::shared_ptr <detail::polymorphic_parser <Input, Output, SkipParser>>
        implementation;
public:
    rule() {}

    template <class Parser> rule (Parser const & parser)
    : implementation (new detail::polymorphic_parser_implementation <
        Input, Output, SkipParser, Parser> (parser)) {}
};

struct rule_tag;
template <class Input, class Output, class SkipParser>
    struct decayed_parser_tag <rule <Input, Output, SkipParser>>
{ typedef rule_tag type; };

namespace operation {

    template <> struct parse <rule_tag> {
        template <class Policy, class RuleInput, class Output, class SkipParser,
                class ActualInput>
        // Always generate a return type, to prevent strange errors.
            explicit_outcome <Output, ActualInput> operator() (
                Policy const & outside_policy,
                rule <RuleInput, Output, SkipParser> const & parser,
                ActualInput const & input) const
        {
            // Generate an error here.
            static_assert (std::is_same <RuleInput, ActualInput>::value,
                "The rule parser can only parse with a fixed Input type");
            detail::opaque_policy <RuleInput, SkipParser> inside_policy (
                outside_policy);
            return parser.implementation->parse_from (inside_policy, input);
        }
    };

    template <> struct describe <rule_tag> {
        template <class Rule> const char * operator() (Rule const &) const
        { return "rule (opaque)"; }
    };

} // namespace operation

class rule_explicit_skip_parser : parse_policy::direct {
public:
    template <class OriginalPolicy>
        rule_explicit_skip_parser (OriginalPolicy const &) {}

    // This is not supposed to be called.
    void skip_parser();
};

} // namespace parse_ll

#endif  // PARSE_LL_RULE_HPP_INCLUDED_HPP

