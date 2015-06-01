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

#ifndef PARSE_LL_CORE_OUTCOME_FAILED_HPP_INCLUDED
#define PARSE_LL_CORE_OUTCOME_FAILED_HPP_INCLUDED

#include "rime/core.hpp"
#include "rime/assert.hpp"

#include "./core.hpp"

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
    { rime::assert_ (!success (other)); }
};

namespace operation {

    template <> struct success <failed>
    { rime::false_type operator() (failed) const { return rime::false_; } };

    // "output" and "failed" remain unimplemented.

} // namespace operation

} // namespace parse_ll

#endif // PARSE_LL_CORE_OUTCOME_FAILED_HPP_INCLUDED

