// SPDX-FileCopyrightText: Copyright 2012 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

#ifndef PARSE_LL_BASE_FWD_HPP
#define PARSE_LL_BASE_FWD_HPP

#include <type_traits>
#include <boost/mpl/not.hpp>

namespace parse_ll {

template <class Parser, class Enable = void> struct decayed_parser_tag;
template <class Parser, class Enable = void> struct parser_tag;
template <class Parser> struct is_parser;

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
template <class Input, class Output = void, class SkipParser = void>
struct rule;
struct named_parser;

namespace parse_policy {

    struct direct;
    template <class OriginalParse> struct no_skip_policy;
    template <class SkipParser, class OriginalParse> struct skip_policy;

}  // namespace parse_policy

namespace operation {

    template <class ParserTag, typename Enable = void> struct parse;
    template <class ParserTag, typename Enable = void> struct skip_over;
    template <class ParserTag, typename Enable = void> struct describe;
    template <class Outcome, typename Enable = void> struct success;
    template <class Outcome, typename Enable = void> struct output;
    template <class Outcome, typename Enable = void> struct rest;

    struct unimplemented {};
    template <class Operation> struct is_implemented
    : boost::mpl::not_<std::is_base_of<unimplemented, Operation>> {};

}  // namespace operation

}  // namespace parse_ll

#endif  // PARSE_LL_BASE_FWD_HPP
