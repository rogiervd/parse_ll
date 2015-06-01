/*
Copyright 2012, 2013 Rogier van Dalen.

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
        RETURNS (apply::success <Outcome>() (std::forward <Outcome> (outcome)));
    };

    struct output {
        template <class Outcome> auto operator() (Outcome && outcome) const
        RETURNS (apply::output <Outcome>() (std::forward <Outcome> (outcome)));
    };

    struct rest {
        template <class Outcome> auto operator() (Outcome && outcome) const
        RETURNS (apply::rest <Outcome>() (std::forward <Outcome> (outcome)));
    };

} // namespace callable

static const auto success = callable::success();
static const auto output = callable::output();
static const auto rest = callable::rest();

} // namespace parse_ll

#endif // PARSE_LL_CORE_OUTCOME_CORE_HPP_INCLUDED

