/*
Copyright 2012 Rogier van Dalen.

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
Define the no_skip directive, which disables any skip parser for the sub-parser.
*/

#ifndef PARSE_LL_BASE_NO_SKIP_HPP_INCLUDED
#define PARSE_LL_BASE_NO_SKIP_HPP_INCLUDED

#include "detail/directive.hpp"
#include "fail.hpp"

namespace parse_ll {

namespace parse_policy {

    template <class OriginalPolicy> struct no_skip_policy
    : public OriginalPolicy
    {
    public:
        explicit no_skip_policy (OriginalPolicy const & original_policy_)
        : OriginalPolicy (original_policy_) {}

        fail_parser const & skip_parser() const { return fail; }

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

