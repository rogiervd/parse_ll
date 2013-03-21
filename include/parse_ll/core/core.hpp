/*
Copyright 2012, 2013 Rogier van Dalen.

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
Base classes for parser.
*/

#ifndef PARSE_LL_CORE_CORE_HPP_INCLUDED
#define PARSE_LL_CORE_CORE_HPP_INCLUDED

#include <utility>
#include <type_traits>

#include <boost/mpl/if.hpp>
#include <boost/mpl/not.hpp>
#include <boost/utility/enable_if.hpp>

#include "utility/returns.hpp"

#include "rime/core.hpp"
#include "range/core.hpp"

#include "fwd.hpp"
#include "detail/parser_base.hpp"
#include "outcome/core.hpp"
#include "fail.hpp"

namespace parse_ll {

struct not_a_parser_tag;

template <class Parser, class Enable /* = void */> struct decayed_parser_tag
{ typedef not_a_parser_tag type; };

template <class Parser, class Enable /* = void */> struct parser_tag
: decayed_parser_tag <typename std::decay <Parser>::type> {};

namespace detail {

    // Check whether parser_tag <Parser> is defined.
    template <class Parser> struct has_parser_tag
    : boost::mpl::not_ <std::is_same <
        typename parser_tag <Parser>::type, not_a_parser_tag>> {};

    // Check whether Parser derives from parser_base <...>.
    template <class Parser> struct has_parser_base_impl {
        template <class Parser2>
            static rime::true_type test_base (parser_base <Parser2> const *);
        static rime::false_type test_base (void *);

        typedef decltype (test_base (
            std::declval <typename std::decay <Parser>::type *>())) type;
    };

    template <class Parser> struct has_parser_base
    : has_parser_base_impl <Parser>::type {};

} // namespace detail

template <class Parser> struct is_parser
: detail::has_parser_tag <Parser>
{
    static_assert (detail::has_parser_tag <Parser>::value ==
        detail::has_parser_base <Parser>::value,
        "All parsers should have a tag and derive from parser_base.");
};

/*** operation ***/
namespace operation {

    // Will be called with Policy, Parser and Input.
    template <class ParserTag, typename Enable /* = void*/>
        struct parse : unimplemented {};

    /**
    Apply the parser as a skip parser.
    That is, parse with it, and if it is successful, then return the rest of the
    input; if it is not successful, then return the original input.
    A default implementation, which always works but may not be optimal, is
    provided.
    Will be called with Policy, Parser and Input.
    */
    template <class ParserTag, typename Enable /* = void*/>
        struct skip_over;

    /**
    Give a textual representation of a parser.
    The representation is not nested.
    Specialise this for every Parser class.
    */
    template <class ParserTag, typename Enable /*= void*/> struct describe;

} // namespace operation

namespace apply {

    template <class ... Arguments> struct parse;

    template <class ... Arguments> struct skip_over;

    template <class ... Arguments> struct describe;
    template <class Parser> struct describe <Parser>
    : operation::describe <typename parser_tag <Parser>::type>
    {
        static_assert (is_parser <Parser>::value,
            "describe (p): p must be a parser.");
    };

} // namespace apply

namespace has {

    template <class ... Arguments> struct parse
    : operation::is_implemented <apply::parse <Arguments ...> > {};

    template <class ... Arguments> struct skip_over
    : operation::is_implemented <apply::parse <Arguments ...> > {};

    template <class ... Arguments> struct describe
    : operation::is_implemented <apply::describe <Arguments ...> > {};

} // namespace has

namespace callable {

    namespace detail {
        template <template <class ...> class Apply> struct generic
        {
            template <class ... Arguments>
                auto operator() (Arguments && ... arguments) const
            RETURNS (Apply <Arguments ...>() (
                std::forward <Arguments> (arguments) ...))
        };
    } // namespace detail

    struct parse : detail::generic <apply::parse> {};
    struct skip_over : detail::generic <apply::skip_over> {};
    struct describe : detail::generic <apply::describe> {};

} // namespace callable

namespace parse_policy {

    /**
    Default parse policy.
    It does not use a skip parser.
    */
    struct direct {
        /**
        Perform the parse and return the return type.
        This works by calling a default-constructed instance of
        Apply::apply <Policy, Parser, Input> with (Policy, Parser, Input).
        The return type depends on the type that Apply::apply <...> returns.

        Because of its extensive compile-time polymorphism, it is not possible
        for run-time polymorphic parsers like rule<> to propagate apply_parse
        inside.
        */
        template <class Apply, class Policy, class Parser, class Input>
            auto apply_parse (Policy const & policy, Parser const & parser,
                Input && input) const
        RETURNS (typename Apply::template
            apply <Policy const &, Parser const &, Input>() (
                policy, parser, std::forward <Input> (input)))

        /**
        \return The skip parser to use between elements
        */
        fail_parser const & skip_parser() const { return fail; }
    };

} // parse_policy

static const auto parse = callable::parse();
static const auto skip_over = callable::skip_over();
static const auto describe = callable::describe();

namespace apply {

    namespace parse_detail {

        template <class Apply, class Arguments, class Enable = void>
        struct arrange_parse_parameters;

        // Policy, Parser, and Input which is a view: forward.
        template <class Apply, class Policy, class Parser, class Input>
            struct arrange_parse_parameters <
                Apply, meta::vector <Policy, Parser, Input>,
                typename boost::enable_if <range::is_view <Input>>::type>
        {
            static_assert (is_parser <Parser>::value,
                "parse (policy, p, input): p must be a parser.");

            auto operator() (Policy && policy, Parser && parser, Input && input)
                const
            RETURNS (policy.template apply_parse <Apply> (
                policy, std::forward <Parser> (parser),
                std::forward <Input> (input)))
        };

        // Policy, Parser, and Input which is not a view: call view (input).
        template <class Apply, class Policy, class Parser, class Input>
            struct arrange_parse_parameters <
                Apply, meta::vector <Policy, Parser, Input>,
                typename boost::disable_if <range::is_view <Input>>::type>
        {
            typedef typename range::result_of::view <Input>::type view_type;
            arrange_parse_parameters <Apply,
                meta::vector <Policy, Parser, view_type>> implementation;

            auto operator() (Policy && policy, Parser && parser, Input && input)
                const
            RETURNS (implementation (std::forward <Policy> (policy),
                std::forward <Parser> (parser),
                range::view (std::forward <Input> (input))))
        };

        // Just Parser, and Input: prepend the default policy.
        template <class Apply, class Parser, class Input>
            struct arrange_parse_parameters <
                Apply, meta::vector <Parser, Input>>
        {
            static_assert (is_parser <Parser>::value,
                "parse (p, input): p must be a parser.");

            arrange_parse_parameters <Apply, meta::vector <
                    parse_policy::direct const &, Parser, Input>>
                implementation;

            auto operator() (Parser && parser, Input && input) const
            RETURNS (implementation (parse_policy::direct(),
                std::forward <Parser> (parser), std::forward <Input> (input)))
        };

        template <template <class, class> class Operation> struct apply_parse {
            template <class Policy, class Parser, class Input> struct apply
            : Operation <typename parser_tag <Parser>::type, void> {};
        };

    } // namespace parse_detail

    template <class ... Arguments> struct parse
    : parse_detail::arrange_parse_parameters <
        parse_detail::apply_parse <operation::parse>,
        meta::vector <Arguments...>> {};

    template <class ... Arguments> struct skip_over
    : parse_detail::arrange_parse_parameters <
        parse_detail::apply_parse <operation::skip_over>,
        meta::vector <Arguments...>> {};

} // namespace apply

namespace operation {

    namespace skip_over_detail {

        template <class ParserTag> struct skip_over_default {
            template <class Policy, class Parser, class Input>
                typename std::decay <Input>::type operator() (
                    Policy const & policy, Parser const & parser,
                    Input && input) const
            {
                auto outcome = parse_ll::parse (
                    policy, parser, std::forward <Input> (input));
                if (parse_ll::success (outcome))
                    return parse_ll::rest (std::move (outcome));
                else
                    return std::forward <Input> (input);
            }
        };

    } // namespace skip_over_detail

    /**
    Apply the parser as a skip parser.
    That is, parse with it, and if it is successful, then return the rest of the
    input; if it is not successful, then return the original input.
    A default implementation, which always works but may not be optimal, is
    provided.
    Will be called with Policy, Parser and Input.
    */
    template <class ParserTag, typename Enable /* = void*/>
        struct skip_over
    : boost::mpl::if_ <is_implemented <parse <ParserTag>>,
        skip_over_detail::skip_over_default <ParserTag>, unimplemented>::type
    {};

} // namespace operation

namespace detail {

    /**
    Find the unqualified type of the outcome.
    */
    template <class Policy, class Parser, class Input> struct parser_outcome
    : std::decay <typename std::result_of <callable::parse (
        Policy, Parser, Input)>::type> {};

    template <class Outcome> struct outcome_output {
        typedef decltype (::parse_ll::output (std::declval <Outcome>())) type;
    };

    /**
    Find the output of a parser, i.e. the output of the outcome of the parser.
    This assumes the outcome is used as an lvalue.
    */
    template <class Policy, class Parser, class Input> struct parser_output
    : outcome_output <typename parser_outcome <Policy, Parser, Input>::type &>
    {};

} // namespace detail

} // namespace parse_ll

#endif // PARSE_LL_CORE_CORE_HPP_INCLUDED

