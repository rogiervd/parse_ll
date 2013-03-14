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

