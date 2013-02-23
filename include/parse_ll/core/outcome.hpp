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
An outcome type that contains the output and the remaining input.
*/

#ifndef PARSE_LL_BASE_OUTCOME_SIMPLE_HPP_INCLUDED
#define PARSE_LL_BASE_OUTCOME_SIMPLE_HPP_INCLUDED

// GCC spurious warning from boost::optional with -O1 or higher.
// http://gcc.gnu.org/bugzilla/show_bug.cgi?id=47679
#if BOOST_WORKAROUND(__GNUC__, == 4) && (__GNUC_MINOR__ == 6)
// Retain -Wuninitialized in debug mode.
#ifdef __OPTIMIZE__
#pragma GCC diagnostic ignored "-Wuninitialized"
#endif
#endif

#include <utility>
#include <type_traits>

#include <boost/optional.hpp>

#include "core.hpp"

namespace parse_ll {

/**
Empty class that indicates an unsuccessful outcome of a parse.
*/
struct failed {
    /**
    Constructor.
    Because a failed parse returns no information, this is empty.
    */
    failed() {}

    /**
    Construct from other outcome.
    \pre !success (other)
    */
    template <class OtherOutcome> failed (OtherOutcome const & other)
    { assert (!success (other)); }
};

/**
Outcome class that is always successful and keeps the output and the input
directly in memory.
*/
template <class Output, class Input> struct successful {
    static_assert (!std::is_const <Input>::value,
        "Input should not be a reference or const");
    static_assert (!std::is_reference <Input>::value,
        "Input should not be a reference or const");

    Output output;
    Input rest;
public:
    /**
    Construct with explicit output and rest.
    This is not available when Output is void.
    */
    template <class OtherOutput, class OtherInput>
    successful (OtherOutput && output, OtherInput && rest)
    : output (std::forward <OtherOutput> (output)),
        rest (std::forward <OtherInput> (rest)) {}

    /**
    Construct with explicit rest, but default-constructed output.
    This is particularly useful because it generalises to when Output is void.
    */
    explicit successful (Input const & rest) : output(), rest (rest) {}
    // These to ambiguate from below constructor.
    explicit successful (Input & rest) : output(), rest (rest) {}
    explicit successful (Input && rest) : output(), rest (std::move (rest)) {}

    /**
    Construct from other outcome.
    output (other) must be convertible to Output.
    If Output is void, the type of output (other) is irrelevant.
    \pre success (other)
    \todo
    In some cases, other can hold on to its input range for longer than
    necessary.
    To prevent that, this would have to somehow be specialised.
    */
    template <class OtherOutcome> successful (OtherOutcome && other)
    : output (::parse_ll::output (other)),
        rest (::parse_ll::rest (std::forward <OtherOutcome> (other)))
    {
        // This assertion is almost never triggered, because output (other)
        // will have already caused a crash.
        // Sorry.
        assert (success (other));
    }
};

// Specialisation for Output = void.
template <class Input> struct successful <void, Input> {
    static_assert (!std::is_const <Input>::value,
        "Input should not be a reference or const");
    static_assert (!std::is_reference <Input>::value,
        "Input should not be a reference or const");

    Input rest;
public:
    explicit successful (Input const & rest) : rest (rest) {}
    explicit successful (Input & rest) : rest (rest) {}
    explicit successful (Input && rest) : rest (std::move (rest)) {}

    template <class OtherOutcome> successful (OtherOutcome && other)
    : rest (::parse_ll::rest (std::forward <OtherOutcome> (other)))
    { assert (success (other)); }
};

/**
Outcome class that, if successful, keeps the output and the input directly in
memory.
Useful for eager parsers.
\todo Optimise using move constructors and in-place construction of the result.
\todo This (and outcomes in general) is not necessarily assignable, is it?
    Otherwise something must be done with reference types for Output.
*/
template <class Output, class Input> struct simple_outcome {
    static_assert (!std::is_const <Input>::value,
        "Input should not be a reference or const");
    static_assert (!std::is_reference <Input>::value,
        "Input should not be a reference or const");

    typedef successful <Output, Input> successful_outcome;
    boost::optional <successful_outcome> result;
public:
    /**
    Construct as successful.
    */
    template <class OtherOutput, class OtherInput>
    simple_outcome (OtherOutput && output, OtherInput && rest)
    : result (successful_outcome (
        std::forward <OtherOutput> (output), std::forward <OtherInput> (rest)))
    {}

    /**
    Construct with explicit rest, but default-constructed output.
    This is useful because it generalises to when Output is void.
    */
    explicit simple_outcome (Input const & rest)
    : result (successful_outcome (rest)) {}
    explicit simple_outcome (Input & rest)
    : result (successful_outcome (rest)) {}
    explicit simple_outcome (Input && rest)
    : result (successful_outcome (std::forward <Input> (rest))) {}

    /**
    \todo Use in-place construction of the optional?
    */
    template <class OtherOutcome>
        simple_outcome (OtherOutcome && other)
    : result (success (other) ?
        boost::optional <successful_outcome> (
            successful_outcome (std::forward <OtherOutcome> (other)))
        : boost::optional <successful_outcome>()) {}

    // Specialisation of the above for the known-failed case.
    // Otherwise, the compiler tries (and fails) to generate a call to
    // output (other).
    simple_outcome (failed const &) : result() {}
    simple_outcome (failed &) : result() {}
    simple_outcome (failed &&) : result() {}

    template <class Other> simple_outcome & operator= (Other const &) = delete;
};

namespace operation {

    /**** failed ****/

    template <> struct success <failed>
    { bool operator() (failed) const { return false; } };

    // \todo The following cannot be implemented, yet success() cannot indicate
    // this at compile time. Fix with Rime when that comes around.
    template <> struct output <failed> {};
    template <> struct rest <failed> {};

    /**** successful ****/

    template <class Output, class Input>
        struct success <successful <Output, Input>>
    {
        bool operator() (successful <Output, Input> const &) const
        { return true; }
    };

    template <class Output, class Input>
        struct output <successful <Output, Input>>
    {
        Output operator() (successful <Output, Input> const & outcome) const
        { return outcome.output; }
    };
    template <class Input> struct output <successful <void, Input>>
    {
        void operator() (successful <void, Input> const &) const;
    };

    template <class Output, class Input>
        struct rest <successful <Output, Input>>
    {
        Input operator() (successful <Output, Input> const & outcome) const
        { return outcome.rest; }
    };

    /**** simple_outcome ****/

    template <class Output, class Input>
        struct success <simple_outcome <Output, Input>>
    {
        bool operator() (simple_outcome <Output, Input> const & outcome) const
        { return bool (outcome.result); }
    };

    template <class Output, class Input>
        struct output <simple_outcome <Output, Input>>
    {
        Output operator() (simple_outcome <Output, Input> const & outcome) const
        { return ::parse_ll::output (outcome.result.get()); }
    };
    template <class Input> struct output <simple_outcome <void, Input>>
    {
        void operator() (simple_outcome <void, Input> const &) const;
    };

    template <class Output, class Input>
        struct rest <simple_outcome <Output, Input>>
    {
        Input operator() (simple_outcome <Output, Input> const & outcome) const
        { return ::parse_ll::rest (outcome.result.get()); }
    };

} // namespace operation

} // namespace parse_ll

#endif // PARSE_LL_BASE_OUTCOME_SIMPLE_HPP_INCLUDED

