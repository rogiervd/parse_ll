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
An outcome type that contains the output and the remaining input.
*/

#ifndef PARSE_LL_CORE_OUTCOME_EXPLICIT_HPP_INCLUDED
#define PARSE_LL_CORE_OUTCOME_EXPLICIT_HPP_INCLUDED

#include <utility>
#include <type_traits>

// GCC spurious warning from boost::optional with -O1 or higher.
// http://gcc.gnu.org/bugzilla/show_bug.cgi?id=47679
#if BOOST_WORKAROUND(__GNUC__, == 4) && (__GNUC_MINOR__ == 6)
// Retain -Wuninitialized in debug mode.
#ifdef __OPTIMIZE__
#pragma GCC diagnostic ignored "-Wuninitialized"
#endif
#endif

#include <boost/optional.hpp>

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
template <class Output, class Input> struct explicit_outcome {
    typedef successful <Output, Input> successful_outcome;
    boost::optional <successful_outcome> result;
public:
    /**
    Construct as successful.
    */
    template <class OtherOutput, class OtherInput>
    explicit_outcome (OtherOutput && output, OtherInput && rest)
    : result (successful_outcome (
        std::forward <OtherOutput> (output), std::forward <OtherInput> (rest)))
    {}

    /**
    Construct with explicit rest, but default-constructed output.
    This is useful because it generalises to when Output is void.
    */
    explicit explicit_outcome (Input const & rest)
    : result (successful_outcome (rest)) {}
    explicit explicit_outcome (Input & rest)
    : result (successful_outcome (rest)) {}
    explicit explicit_outcome (Input && rest)
    : result (successful_outcome (std::forward <Input> (rest))) {}

    /**
    \todo Use in-place construction of the optional?
    */
    template <class OtherOutcome>
        explicit_outcome (OtherOutcome && other)
    : result (success (other) ?
        boost::optional <successful_outcome> (
            successful_outcome (std::forward <OtherOutcome> (other)))
        : boost::optional <successful_outcome>()) {}

    // Specialisation of the above for the known-failed case.
    // Otherwise, the compiler tries (and fails) to generate a call to
    // output (other).
    explicit_outcome (failed const &) : result() {}
    explicit_outcome (failed &) : result() {}
    explicit_outcome (failed &&) : result() {}

    template <class Other> explicit_outcome & operator = (Other const &)
        = delete;
};

namespace operation {

    template <class Output, class Input>
        struct success <explicit_outcome <Output, Input>>
    {
        bool operator() (explicit_outcome <Output, Input> const & outcome) const
        { return bool (outcome.result); }
    };

    template <class Output, class Input>
        struct output <explicit_outcome <Output, Input>>
    {
        Output operator() (explicit_outcome <Output, Input> const & outcome)
            const
        { return ::parse_ll::output (outcome.result.get()); }

        Output operator() (explicit_outcome <Output, Input> const && outcome)
            const
        { return ::parse_ll::output (std::move (outcome.result.get())); }
    };
    template <class Input> struct output <explicit_outcome <void, Input>>
    { void operator() (explicit_outcome <void, Input> const &) const; };

    template <class Output, class Input>
        struct rest <explicit_outcome <Output, Input>>
    {
        Input operator() (explicit_outcome <Output, Input> const & outcome)
            const
        { return ::parse_ll::rest (outcome.result.get()); }
        Input operator() (explicit_outcome <Output, Input> && outcome) const
        { return ::parse_ll::rest (std::move (outcome.result.get())); }
    };

} // namespace operation

} // namespace parse_ll

#endif // PARSE_LL_CORE_OUTCOME_EXPLICIT_HPP_INCLUDED

