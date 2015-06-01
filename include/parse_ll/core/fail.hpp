/*
Copyright 2013 Rogier van Dalen.

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
Define a parser that always fails.
*/

#ifndef PARSE_LL_FAIL_HPP_INCLUDED
#define PARSE_LL_FAIL_HPP_INCLUDED

#include "utility/returns.hpp"

#include "fwd.hpp"
#include "detail/parser_base.hpp"
// Do not include core.hpp: this file is included in core.hpp.
#include "outcome/failed.hpp"

namespace parse_ll {

// Parser that always fails.
struct fail_parser : parser_base <fail_parser> {};
static const auto fail = fail_parser();

struct fail_parser_tag;

template <> struct decayed_parser_tag <fail_parser>
{ typedef fail_parser_tag type; };

namespace operation {

    template <> struct parse <fail_parser_tag> {
        template <class Policy, class Input> auto operator() (
            Policy const &, fail_parser const &, Input &&) const
        RETURNS (failed());
    };

    template <> struct skip_over <fail_parser_tag> {
        template <class Policy, class Input> Input && operator() (
            Policy const &, fail_parser const &, Input && input) const
        { return std::forward <Input> (input); }
    };

    template <> struct describe <fail_parser_tag> {
        const char * operator() (fail_parser const &) const { return "fail"; }
    };

} // namespace operation

} // namespace parse_ll

#endif // PARSE_LL_FAIL_HPP_INCLUDED

