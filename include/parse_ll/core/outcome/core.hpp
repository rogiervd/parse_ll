// SPDX-FileCopyrightText: Copyright 2012, 2013 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

/** \file
Define operations for outcomes.
*/

#ifndef PARSE_LL_CORE_OUTCOME_CORE_HPP_INCLUDED
#define PARSE_LL_CORE_OUTCOME_CORE_HPP_INCLUDED

#include <type_traits>
#include <utility>

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
    template <class Outcome, typename Enable /* = void */> struct success
    : unimplemented {};

    /**
    Implementation of "output()".
    Specialise this for every Outcome class.
    This will never be called if the result type of operator() is declared to
    return void.
    */
    template <class Outcome, typename Enable /* = void */> struct output
    : unimplemented {};
    template <class Outcome, typename Enable /* = void */> struct rest
    : unimplemented {};

    namespace output_detail {

        template <class Outcome> struct output_result
        {
            typedef decltype(std::declval<output<Outcome>>()(
                std::declval<Outcome>())) type;
        };

        /**
        Implementation of "output()" used if it would return void.
        This simply short-circuits.
        */
        template <class Outcome, typename Enable = void> struct output_if_void
        {
            void operator()(Outcome const &) {}
        };

    }  // namespace output_detail

    /**
    Check whether the normal output returns void.
    If so, derive from output_if_void, which does nothing.
    Otherwise, derive from output.
    */
    template <class Outcome, typename Enable = void> struct output_even_if_void
    : boost::mpl::if_<
          std::is_same<
              typename output_detail::output_result<Outcome>::type, void>,
          output_detail::output_if_void<Outcome>, output<Outcome>>::type {};

}  // namespace operation

namespace apply {

    template <class Outcome> struct success
    : operation::success<typename std::decay<Outcome>::type> {};

    template <class Outcome> struct output
    : operation::output_even_if_void<typename std::decay<Outcome>::type> {};

    template <class Outcome> struct rest
    : operation::rest<typename std::decay<Outcome>::type> {};

}  // namespace apply

namespace has {

    template <class Outcome> struct success
    : operation::is_implemented<apply::success<Outcome>> {};
    template <class Outcome> struct output
    : operation::is_implemented<apply::output<Outcome>> {};
    template <class Outcome> struct rest
    : operation::is_implemented<apply::rest<Outcome>> {};

}  // namespace has

namespace callable {

    struct success
    {
        template <class Outcome> auto operator()(Outcome && outcome) const
            RETURNS(apply::success<Outcome>()(std::forward<Outcome>(outcome)));
    };

    struct output
    {
        template <class Outcome> auto operator()(Outcome && outcome) const
            RETURNS(apply::output<Outcome>()(std::forward<Outcome>(outcome)));
    };

    struct rest
    {
        template <class Outcome> auto operator()(Outcome && outcome) const
            RETURNS(apply::rest<Outcome>()(std::forward<Outcome>(outcome)));
    };

}  // namespace callable

static auto const success = callable::success();
static auto const output = callable::output();
static auto const rest = callable::rest();

}  // namespace parse_ll

#endif  // PARSE_LL_CORE_OUTCOME_CORE_HPP_INCLUDED
