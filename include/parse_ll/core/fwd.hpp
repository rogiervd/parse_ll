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

#ifndef PARSE_LL_BASE_FWD_HPP
#define PARSE_LL_BASE_FWD_HPP

namespace parse_ll {

// These are required for core.hpp
struct nothing_parser;
template <class Derived> struct parser_base;
template <class SubParser> struct repeat_parser;
template <class Parser, class Actor> struct transform_parser;
template <class SubParser> struct optional_parser;
template <class Parser1, class Parser2> struct alternative_parser;
template <bool Expect, class Parser1, class Parser2> struct sequence_parser;
template <class Parser1, class Parser2> struct difference_parser;

// These are useful for other programs.
template <class Input, class Output = void> struct rule;
struct named_parser;

namespace parse_policy {
    struct direct;
    template <class OriginalParse> struct no_skip_policy;
    template <class SkipParser, class OriginalParse> struct skip_policy;
} // namespace parse_policy

} // namespace parse_ll

#endif  // PARSE_LL_BASE_FWD_HPP
