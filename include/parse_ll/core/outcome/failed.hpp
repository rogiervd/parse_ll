// SPDX-FileCopyrightText: Copyright 2012, 2013 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

/** \file
An outcome type that contains the output and the remaining input.
*/

#ifndef PARSE_LL_CORE_OUTCOME_FAILED_HPP_INCLUDED
#define PARSE_LL_CORE_OUTCOME_FAILED_HPP_INCLUDED

#include "rime/assert.hpp"
#include "rime/core.hpp"

#include "./core.hpp"

namespace parse_ll {

/**
Empty class that indicates an unsuccessful outcome of a parse.
*/
struct failed
{
    /**
    Constructor.
    Because a failed parse returns no information, this is empty.
    */
    failed() {}

    /**
    Construct from other outcome.
    \pre !success (other)
    */
    template <class OtherOutcome> failed(OtherOutcome const & other)
    {
        rime::assert_(!success(other));
    }
};

namespace operation {

    template <> struct success<failed>
    {
        rime::false_type operator()(failed) const { return rime::false_; }
    };

    // "output" and "failed" remain unimplemented.

}  // namespace operation

}  // namespace parse_ll

#endif  // PARSE_LL_CORE_OUTCOME_FAILED_HPP_INCLUDED
