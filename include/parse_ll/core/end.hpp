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
Defines an "end" parser, which only succeeds at the end of the input.
*/

#ifndef PARSE_LL_BASE_END_HPP_INCLUDED
#define PARSE_LL_BASE_END_HPP_INCLUDED

#include "core.hpp"
#include "outcome/explicit.hpp"
#include "outcome/failed.hpp"

#include "range/core.hpp"

namespace parse_ll {

struct end_parser : parser_base <end_parser> {};
static const auto end = end_parser();

struct end_parser_tag;
template <> struct decayed_parser_tag <end_parser>
{ typedef end_parser_tag type; };

namespace operation {

    template <> struct parse <end_parser_tag> {
        template <class Policy, class Input> explicit_outcome <void, Input>
            operator() (Policy const &, end_parser, Input const & input) const
        {
            if (::range::empty (input))
                return explicit_outcome <void, Input> (input);
            else
                return failed();
        }
    };

    template <> struct describe <end_parser_tag> {
        const char * operator() (end_parser const &) const
        { return "end"; }
    };


} // namespace operation

} // namespace parse_ll

#endif  // PARSE_LL_BASE_END_HPP_INCLUDED

