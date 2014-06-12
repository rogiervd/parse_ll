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

/**
Define a parser wrapper that makes compiler errors more meaningful.
*/

#ifndef PARSE_LL_BASE_NAMED_HPP_INCLUDED
#define PARSE_LL_BASE_NAMED_HPP_INCLUDED

#include "utility/returns.hpp"

#include "core.hpp"
#include <type_traits>
#include <boost/utility/enable_if.hpp>

namespace parse_ll {

/**
Base class for producing parsers that wrap other parsers.
Using named_parser is never necessary, but it is convenient for debugging.
For example, the predefined parser for "float" would have a five-line type if it
were not a named parser "float_parser".

A class deriving from this should have a member "implementation()" which is the
actual parser to be used, and a member "description()" which returns a short
description of the parser.
*/
struct named_parser {};

struct named_parser_tag;
template <class Parser> struct decayed_parser_tag <Parser,
    typename boost::enable_if <std::is_base_of <named_parser, Parser>>::type>
{ typedef named_parser_tag type; };

namespace operation {
    template <> struct parse <named_parser_tag> {
        template <class Policy, class Parser, class Input>
            auto operator() (Policy const & policy,
                Parser const & parser, Input const & input) const
        RETURNS (parse_ll::parse (policy, parser.implementation(), input));
    };

    template <> struct describe <named_parser_tag> {
        template <class Parser> auto operator() (Parser const & parser) const
        RETURNS (parser.description());
    };
}

} // namespace parse_ll

/**
\todo Test separately.

Define a named parser type that wraps the parser expression in the dots.
*/
#define PARSE_LL_DEFINE_NAMED_PARSER_TYPE(name, type_name, ...) \
struct type_name \
: ::parse_ll::parser_base <type_name>, ::parse_ll::named_parser { \
    typedef decltype (__VA_ARGS__) implementation_type; \
    implementation_type implementation_; \
    type_name() : implementation_ (__VA_ARGS__) {} \
    implementation_type const & implementation() const { \
        return implementation_; \
    } \
    const char * description() const { return name; } \
}

/**
Define a named parser type "name_parser" that wraps the parser expression in the
dots, and a static const variable "name" of type "name_parser".
*/
#define PARSE_LL_DEFINE_NAMED_PARSER(name, ...) \
PARSE_LL_DEFINE_NAMED_PARSER_TYPE (#name, name##_parser, __VA_ARGS__); \
static name##_parser const name = name##_parser();

/**
Define a named parser type that wraps the parser expression in the dots and has
one template parameter "template_parameter".
*/
#define PARSE_LL_DEFINE_NAMED_PARSER_TEMPLATE( \
    name, type_name, template_parameter, ...) \
template <typename template_parameter> struct type_name \
: ::parse_ll::parser_base <type_name <template_parameter>>, \
    ::parse_ll::named_parser \
{ \
    typedef decltype (__VA_ARGS__) implementation_type; \
    implementation_type implementation_; \
    type_name() : implementation_ (__VA_ARGS__) {} \
    implementation_type const & implementation() const { \
        return implementation_; \
    } \
    const char * description() const { return #name; } \
}

#endif  // PARSE_LL_BASE_NAMED_HPP_INCLUDED

