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
Define the no_skip directive, which disables any skip parser for the sub-parser.
*/

#ifndef PARSE_LL_BASE_NO_SKIP_HPP_INCLUDED
#define PARSE_LL_BASE_NO_SKIP_HPP_INCLUDED

#include "detail/directive.hpp"

namespace parse_ll {

namespace parse_policy {

    template <class OriginalPolicy> struct no_skip_policy
    : public OriginalPolicy
    {
    public:
        explicit no_skip_policy (OriginalPolicy const & original_policy_)
        : OriginalPolicy (original_policy_) {}

        template <class Input> auto skip (Input && input) const
            RETURNS (std::forward <Input> (input));

        OriginalPolicy const & original_policy() const { return *this; }
    };

} // namespace parse_policy

/**
Wrap a parse function to inhibit skipping.
*/
class convert_policy_no_skip {
    template <class OriginalPolicy> static auto
        convert (OriginalPolicy const & original_policy)
    RETURNS (parse_policy::no_skip_policy <OriginalPolicy> (original_policy));

    // Simple optimisations: do not change a parse that already does not skip:
    // - parse_no_skip
    template <class OriginalPolicy>
        static parse_policy::no_skip_policy <OriginalPolicy> const &
        convert (parse_policy::no_skip_policy <OriginalPolicy> const &
            original_policy)
    { return original_policy; }

    // - parse
    static parse_policy::direct const &
        convert (parse_policy::direct const & original_parse)
    { return original_parse; }

    // Workaround for GCC 4.6:
    template <class SkipParser, class OriginalParse> struct convert_skip_result
    {
        parse_policy::skip_policy <SkipParser, OriginalParse> wrapped_policy;
        typedef decltype (convert (wrapped_policy.original_policy())) type;
    };
    // A skip policy can be ignored.
    template <class SkipParser, class OriginalParse> static auto
        convert (parse_policy::skip_policy <SkipParser, OriginalParse> const &
            wrapped_policy)
    // Instead of RETURNS (convert (wrapped_policy.original_policy()));
    -> typename convert_skip_result <SkipParser, OriginalParse>::type
    { return convert (wrapped_policy.original_policy()); }

    // Workaround for GCC 4.6:
    template <class OriginalPolicy> struct result {
        typedef decltype (convert (std::declval <OriginalPolicy>())) type;
    };
public:
    template <class OriginalPolicy> auto
        operator() (OriginalPolicy const & original_policy) const
    // Instead of RETURNS (convert (original_policy));
    -> typename result <OriginalPolicy>::type
    { return convert (original_policy); }
};

static const auto no_skip = change_policy_directive <convert_policy_no_skip> ();

} // namespace parse_ll

#endif  // PARSE_LL_BASE_NO_SKIP_HPP_INCLUDED

