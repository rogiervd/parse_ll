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
Base classes for parser.
*/

#ifndef PARSE_LL_BASE_BASE_HPP_INCLUDED
#define PARSE_LL_BASE_BASE_HPP_INCLUDED

#include <utility>
#include <type_traits>
#include <boost/mpl/not.hpp>
#include <boost/optional.hpp>

#include "utility/returns.hpp"

#include "range/core.hpp"

#include "fwd.hpp"

namespace parse_ll {

/*** parser ***/

template <class Derived> struct parser_base {
    Derived const * this_() const
    { return static_cast <Derived const *> (this); }

    repeat_parser <Derived> operator* () const {
        return repeat_parser <Derived> (*this_(), 0, -1);
    }
    repeat_parser <Derived> operator+ () const {
        return repeat_parser <Derived> (*this_(), 1, -1);
    }

    optional_parser <Derived> operator- () const {
        return optional_parser <Derived> (*this_());
    }

    template <class OtherParser>
        alternative_parser <Derived, OtherParser>
            operator| (OtherParser const & other) const
    {
        return alternative_parser <Derived, OtherParser> (*this_(), other);
    }

    template <class OtherParser>
        sequence_parser <false, Derived, OtherParser>
            operator >> (OtherParser const & other) const
    {
        return sequence_parser <false, Derived, OtherParser> (*this_(), other);
    }

    /**
    \todo Make sure there's no difference between
        a > b >> c and (a > b) >> c.
    */
    template <class OtherParser>
        sequence_parser <true, Derived, OtherParser>
            operator > (OtherParser const & other) const
    {
        return sequence_parser <true, Derived, OtherParser> (*this_(), other);
    }

    template <class OtherParser>
        difference_parser <Derived, OtherParser>
            operator - (OtherParser const & other) const
    {
        return difference_parser <Derived, OtherParser> (*this_(), other);
    }

    template <class Actor>
    transform_parser <Derived, Actor> operator[] (Actor const & actor) const {
        return transform_parser <Derived, Actor> (*this_(), actor);
    }
};

/*** operation ***/
namespace operation {

    struct unimplemented {};
    template <class Operation> struct is_implemented
    : boost::mpl::not_ <std::is_base_of <unimplemented, Operation> > {};

    // Will be instantiated with unqualified Parse,  Parser and Input.
    template <class Parse, class Parser, class Input, typename Enable = void>
        struct parse : unimplemented {};

    /**
    Give a textual representation of a parser.
    The representation is not nested.
    Specialise this for every Parser class.
    */
    template <class Parser, typename Enable = void> struct describe;

    // Will be instantiated with unqualified Outcome.
    /**
    Implementation of "success()".
    Specialise this for every Outcome class.
    */
    template <class Outcome, typename Enable = void>
        struct success : unimplemented {};
    /**
    Implementation of "output()".
    Specialise this for every Outcome class.
    This will never be called if the result type of operator() is declared to
    return void.
    */
    template <class Outcome, typename Enable = void>
        struct output : unimplemented {};
    template <class Outcome, typename Enable = void>
        struct rest : unimplemented {};

    namespace output_detail {
        template <class Outcome> struct output_result {
            typedef decltype (std::declval <output <Outcome>>() (
                std::declval <Outcome>())) type;
        };

        /**
        Implementation of "output()" used if it would return void.
        This simply short-circuits.
        */
        template <class Outcome, typename Enable = void>
            struct output_if_void
        { void operator() (Outcome const &) {} };

    } // namespace detail

    /**
    Check whether the normal output returns void.
    If so, derive from output_if_void, which does nothing.
    Otherwise, derive from output.
    */
    template <class Outcome, typename Enable = void>
        struct output_even_if_void :
        boost::mpl::if_ <std::is_same <
            typename output_detail::output_result <Outcome>::type, void>,
            output_detail::output_if_void <Outcome>, output <Outcome>>::type {};

} // namespace operation

namespace apply {

    template <class Parse, class Parser, class Input> struct parse
    : operation::parse <typename std::decay <Parse>::type,
        typename std::decay <Parser>::type,
        typename std::decay <Input>::type> {};

    template <class Parser> struct describe
    : operation::describe <typename std::decay <Parser>::type> {};

    template <class Outcome> struct success
    : operation::success <typename std::decay <Outcome>::type> {};

    template <class Outcome> struct output
    : operation::output_even_if_void <typename std::decay <Outcome>::type> {};

    template <class Outcome> struct rest
    : operation::rest <typename std::decay <Outcome>::type> {};

} // namespace apply

namespace has {
    template <class Parse, class Parser, class Input> struct parse
    : operation::is_implemented <apply::parse <Parse, Parser, Input> > {};

    template <class Outcome> struct success
    : operation::is_implemented <apply::success <Outcome> > {};
    template <class Outcome> struct output
    : operation::is_implemented <apply::output <Outcome> > {};
    template <class Outcome> struct rest
    : operation::is_implemented <apply::rest <Outcome> > {};

} // namespace has

namespace callable {

    // Workaround for GCC 4.6, which it refuses to mangle
    // std::declval <Policy>().apply_parse (...).
    // http://gcc.gnu.org/bugzilla/show_bug.cgi?id=48051
    template <class Policy, class WrappedParse, class Parser, class Input>
        struct apply_parse_type {
        Policy policy;
        typedef decltype (policy.apply_parse (std::declval <WrappedParse>(),
            std::declval <Parser>(), std::declval <Input>())) type;
    };

    /**
    Base class that provides a parse function that can hold properties that
    get propagated through the parse.
    It takes a policy argument which determines the behaviour.

    \todo Make it possible to curry this.
    */
    template <class Policy> struct parse : public Policy {
        template <class ... Arguments>
            parse (Arguments && ... arguments)
        : Policy (std::forward <Arguments> (arguments) ...) {}

        /**
        Call apply_parse with the same parse as wrapper_parse.
        */
        template <class Parser, class Input> auto operator() (
            parser_base <Parser> const & parser, Input && input) const
        -> /*decltype (std::declval <Policy>().apply_parse (
            std::declval <parse>(), *parser.this_(), std::declval <Input>()))*/
            typename boost::lazy_enable_if <range::is_view <Input>,
                apply_parse_type <Policy, parse, Parser, Input>>::type
        {
            static_assert (range::is_range <Input>::value,
                "Input must be a range.");
            static_assert (range::is_homogeneous <Input>::value,
                "Input must be a homogeneous range.");
            return this->apply_parse (*this,
                *parser.this_(), std::forward <Input> (input));
        }

        // Make view before passing on
        template <class Parser, class Input>
            typename boost::lazy_disable_if <range::is_view <Input>,
                apply_parse_type <Policy, parse, Parser,
                    typename range::result_of::view <Input>::type>>::type
        operator() (parser_base <Parser> const & parser, Input && input) const
        {
            static_assert (range::is_range <Input>::value,
                "Input must be a range.");
            return (*this) (parser, range::view (std::forward <Input> (input)));
        }

        Policy const & policy() const { return *this; }
    };


    struct describe {
        template <class Parser> auto operator() (Parser const & parser) const
        RETURNS (apply::describe <Parser>() (parser))
    };

    struct success {
        template <class Outcome> auto operator() (Outcome && outcome) const
        RETURNS (apply::success <Outcome>() (std::forward <Outcome> (outcome)))
    };

    struct output {
        template <class Outcome> auto operator() (Outcome && outcome) const
        RETURNS (apply::output <Outcome>() (std::forward <Outcome> (outcome)))
    };

    struct rest {
        template <class Outcome> auto operator() (Outcome && outcome) const
        RETURNS (apply::rest <Outcome>() (std::forward <Outcome> (outcome)))
    };

} // namespace callable

namespace parse_policy {

    /**
    Default parse policy.
    It does not use a skip parser.
    */
    struct direct {
        /**
        Perform the appropriate action for the parse function, propagating a
        potentially different wrapper_parse.

        This default version just calls apply::parse with the appropriate
        parameters.
        */
        template <class WrapperParse, class Parser, class Input>
            auto apply_parse (
                WrapperParse const & wrapper_parse,
                Parser const & parser, Input && input) const
        RETURNS (apply::parse <WrapperParse, Parser, Input>() (
            wrapper_parse, parser, std::forward <Input> (input)))

        // No skipping.
        template <class Input> auto skip (Input && input) const
            RETURNS (std::forward <Input> (input))
    };

} // parse_policy

template <class Policy> inline callable::parse <Policy>
    parse_with (Policy const & policy)
{ return callable::parse <Policy> (policy); }

static const auto parse = parse_with (parse_policy::direct());
static const auto describe = callable::describe();
static const auto success = callable::success();
static const auto output = callable::output();
static const auto rest = callable::rest();

namespace detail {
    /**
    Find the unqualified type of the outcome.
    */
    template <class Parse, class Parser, class Input> struct parser_outcome
    : std::decay <decltype (std::declval <Parse const>() (
        std::declval <Parser const>(), std::declval <Input const>()))> {};

    template <class Outcome> struct outcome_output {
        typedef decltype (::parse_ll::output (std::declval <Outcome>())) type;
    };

    template <class Parse, class Parser, class Input> struct parser_output
    : outcome_output <typename parser_outcome <Parse, Parser, Input>::type> {};
} // namespace detail

} // namespace parse_ll

#endif // PARSE_LL_BASE_BASE_HPP_INCLUDED

