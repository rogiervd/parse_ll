// SPDX-FileCopyrightText: Copyright 2012, 2013 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

/** \file
An outcome type that contains the output and the remaining input.
*/

#ifndef PARSE_LL_CORE_OUTCOME_EXPLICIT_HPP_INCLUDED
#define PARSE_LL_CORE_OUTCOME_EXPLICIT_HPP_INCLUDED

#include <optional>
#include <type_traits>
#include <utility>


#include "utility/returns.hpp"

#include "./core.hpp"
#include "./successful.hpp"

namespace parse_ll {

/**
Outcome class that, if successful, keeps the output and the input directly in
memory.
Useful for eager parsers.
\todo Optimise using move constructors and in-place construction of the result.
\todo This (and outcomes in general) is not necessarily assignable, is it?
    Otherwise something must be done with reference types for Output.
*/
template <class Output, class Input> struct explicit_outcome
{
    typedef successful<Output, Input> successful_outcome;
    std::optional<successful_outcome> result;

public:
    /**
    Construct as successful.
    */
    template <class OtherOutput, class OtherInput>
    explicit_outcome(OtherOutput && output, OtherInput && rest)
    : result(successful_outcome(
          std::forward<OtherOutput>(output), std::forward<OtherInput>(rest)))
    {}

    /**
    Construct with explicit rest, but default-constructed output.
    This is useful because it generalises to when Output is void.
    */
    explicit explicit_outcome(Input const & rest)
    : result(successful_outcome(rest))
    {}
    explicit explicit_outcome(Input & rest) : result(successful_outcome(rest))
    {}
    explicit explicit_outcome(Input && rest)
    : result(successful_outcome(std::forward<Input>(rest)))
    {}

    /**
    \todo Use in-place construction of the optional?
    */
    template <class OtherOutcome> explicit_outcome(OtherOutcome && other)
    : result(
          success(other) ? std::optional<successful_outcome>(successful_outcome(
                               std::forward<OtherOutcome>(other)))
                         : std::optional<successful_outcome>())
    {}

    // Specialisation of the above for the known-failed case.
    // Otherwise, the compiler tries (and fails) to generate a call to
    // output (other).
    explicit_outcome(failed const &) : result() {}
    explicit_outcome(failed &) : result() {}
    explicit_outcome(failed &&) : result() {}

    template <class Other> explicit_outcome & operator=(Other const &) = delete;
};

namespace operation {

    template <class Output, class Input>
    struct success<explicit_outcome<Output, Input>>
    {
        bool operator()(explicit_outcome<Output, Input> const & outcome) const
        {
            return bool(outcome.result);
        }
    };

    template <class Output, class Input>
    struct output<explicit_outcome<Output, Input>>
    {
        Output operator()(explicit_outcome<Output, Input> const & outcome) const
        {
            return ::parse_ll::output(*outcome.result);
        }

        Output operator()(
            explicit_outcome<Output, Input> const && outcome) const
        {
            return ::parse_ll::output(std::move(*outcome.result));
        }
    };
    template <class Input> struct output<explicit_outcome<void, Input>>
    {
        void operator()(explicit_outcome<void, Input> const &) const;
    };

    template <class Output, class Input>
    struct rest<explicit_outcome<Output, Input>>
    {
        Input operator()(explicit_outcome<Output, Input> const & outcome) const
        {
            return ::parse_ll::rest(*outcome.result);
        }
        Input operator()(explicit_outcome<Output, Input> && outcome) const
        {
            return ::parse_ll::rest(std::move(*outcome.result));
        }
    };

}  // namespace operation

}  // namespace parse_ll

#endif  // PARSE_LL_CORE_OUTCOME_EXPLICIT_HPP_INCLUDED
