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
An outcome type that contains the output and the remaining input.
*/

#ifndef PARSE_LL_CORE_OUTCOME_SUCCESSFUL_HPP_INCLUDED
#define PARSE_LL_CORE_OUTCOME_SUCCESSFUL_HPP_INCLUDED

#include <utility>
#include <type_traits>

#include "rime/core.hpp"
#include "rime/assert.hpp"

#include "./core.hpp"

namespace parse_ll {

/**
Outcome class that is always successful and keeps the output and the input
directly in memory.
The type Input must not be a reference or const, and must be assignable.
The type Output must not be a reference or const, and must be assignable.
Alternatively, Output can be of type "void".
They are stored directly.
*/
template <class Output, class Input> struct successful {
    static_assert (!std::is_const <Input>::value,
        "Input should not be a reference or const");
    static_assert (!std::is_reference <Input>::value,
        "Input should not be a reference or const");

    static_assert (!std::is_const <Output>::value,
        "Output should not be a reference or const");
    static_assert (!std::is_reference <Output>::value,
        "Output should not be a reference or const");

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
    */
    template <class OtherOutcome> successful (OtherOutcome && other)
    : output (::parse_ll::output (other)),
        rest (::parse_ll::rest (std::forward <OtherOutcome> (other)))
    {
        // This assertion is almost never triggered, because output (other)
        // will have already caused a crash.
        // Sorry.
        rime::assert_ (success (other));
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
    { rime::assert_ (success (other)); }
};

namespace operation {

    template <class Output, class Input>
        struct success <successful <Output, Input>>
    {
        rime::true_type operator() (successful <Output, Input> const &) const
        { return rime::true_; }
    };

    template <class Output, class Input>
        struct output <successful <Output, Input>>
    {
        Output const & operator() (successful <Output, Input> const & outcome)
            const
        { return outcome.output; }
        Output && operator() (successful <Output, Input> && outcome) const
        { return std::move (outcome.output); }
    };
    // void output: not implemented since this should never be called.
    template <class Input> struct output <successful <void, Input>>
    { void operator() (successful <void, Input> const &) const; };

    template <class Output, class Input>
        struct rest <successful <Output, Input>>
    {
        Input const & operator() (successful <Output, Input> const & outcome)
            const
        { return outcome.rest; }
        Input && operator() (successful <Output, Input> && outcome) const
        { return std::move (outcome.rest); }
    };

} // namespace operation

} // namespace parse_ll

#endif // PARSE_LL_CORE_OUTCOME_SUCCESSFUL_HPP_INCLUDED

