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
Define operations for outcomes.
*/

#ifndef PARSE_LL_CORE_OUTCOME_CORE_HPP_INCLUDED
#define PARSE_LL_CORE_OUTCOME_CORE_HPP_INCLUDED

#include <utility>
#include <type_traits>

#include <boost/mpl/if.hpp>

#include "utility/returns.hpp"

#include "../fwd.hpp"

namespace parse_ll {

struct failed;
template <class Output, class Input> struct successful;
template <class Output, class Input> struct explicit_outcome;

/*** operation ***/
namespace operation {

    // Will be instantiated with unqualified Outcome.
    /**
    Implementation of "success()".
    Specialise this for every Outcome class.
    */
    template <class Outcome, typename Enable /* = void */>
        struct success : unimplemented {};

    /**
    Implementation of "output()".
    Specialise this for every Outcome class.
    This will never be called if the result type of operator() is declared to
    return void.
    */
    template <class Outcome, typename Enable /* = void */>
        struct output : unimplemented {};
    template <class Outcome, typename Enable /* = void */>
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

    } // namespace output_detail

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

    template <class Outcome> struct success
    : operation::success <typename std::decay <Outcome>::type> {};

    template <class Outcome> struct output
    : operation::output_even_if_void <typename std::decay <Outcome>::type> {};

    template <class Outcome> struct rest
    : operation::rest <typename std::decay <Outcome>::type> {};

} // namespace apply

namespace has {

    template <class Outcome> struct success
    : operation::is_implemented <apply::success <Outcome> > {};
    template <class Outcome> struct output
    : operation::is_implemented <apply::output <Outcome> > {};
    template <class Outcome> struct rest
    : operation::is_implemented <apply::rest <Outcome> > {};

} // namespace has

namespace callable {

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

static const auto success = callable::success();
static const auto output = callable::output();
static const auto rest = callable::rest();

} // namespace parse_ll

#endif // PARSE_LL_CORE_OUTCOME_CORE_HPP_INCLUDED

