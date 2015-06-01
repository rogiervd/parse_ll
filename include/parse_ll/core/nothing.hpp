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
Define a parser that parses nothing of the input and always succeeds.
*/

#ifndef PARSE_LL_NOTHING_HPP_INCLUDED
#define PARSE_LL_NOTHING_HPP_INCLUDED

#include "utility/returns.hpp"

#include "core.hpp"
#include "outcome/successful.hpp"

namespace parse_ll {

// Parser that does not parse any input but is always successful.
struct nothing_parser : parser_base <nothing_parser> {};
static const auto nothing = nothing_parser();

struct nothing_parser_tag;

template <> struct decayed_parser_tag <nothing_parser>
{ typedef nothing_parser_tag type; };

namespace operation {

    template <> struct parse <nothing_parser_tag> {
        template <class Parse, class Input> auto operator() (
            Parse const &, nothing_parser const &, Input && input) const
        RETURNS (successful <void, typename std::decay <Input>::type> (
            std::forward <Input> (input)));
    };

    template <> struct describe <nothing_parser_tag> {
        const char * operator() (nothing_parser const &) const
        { return "nothing"; }
    };
} // namespace operation

} // namespace parse_ll

#endif  // PARSE_LL_NOTHING_HPP_INCLUDED

