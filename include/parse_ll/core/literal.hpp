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
Define a parser that accepts a literal and does not output anything.
*/

#ifndef PARSE_LL_LITERAL_HPP_INCLUDED
#define PARSE_LL_LITERAL_HPP_INCLUDED

#include <string>

#include "range/core.hpp"

#include "fwd.hpp"
#include "core.hpp"
#include "outcome/failed.hpp"
#include "outcome/explicit.hpp"

namespace parse_ll {

/**
Literal must be a range.
It can only be a view if the underlying range is guaranteed to stay in memory
throughout the lifetime of the literal_parser object.
*/
template <class Literal> struct literal_parser
: public parser_base <literal_parser <Literal> >
{
    Literal literal;
public:
    literal_parser (Literal const & literal) : literal (literal) {}
};

struct literal_parser_tag;

template <class Literal> struct decayed_parser_tag <literal_parser <Literal>>
{ typedef literal_parser_tag type; };

inline literal_parser <std::string> literal (char c) {
    return literal_parser <std::string> (std::string (1, c));
}

inline literal_parser <std::string> literal (std::string const & s) {
    return literal_parser <std::string> (s);
}

namespace operation {

    template <> struct parse <literal_parser_tag> {
        template <class Policy, class Literal, class Input>
        explicit_outcome <void, Input> operator() (Policy const &,
            literal_parser <Literal> const & parser, Input input) const
        {
            using ::range::empty; using ::range::first; using ::range::drop;
            auto literal = ::range::view (parser.literal);
            while (!empty (literal) && !empty (input)) {
                if (! (first (literal) == first (input)))
                    return failed();
                literal = drop (literal);
                input = drop (input);
            }
            if (empty (literal))
                // We went through the whole literal: success.
                return explicit_outcome <void, Input> (input);
            else
                return failed();
        }
    };

    template <> struct describe <literal_parser_tag> {
        template <class Parser> const char * operator() (Parser const &) const
        { return "literal"; }
    };

} // namespace operation

} // namespace parse_ll

#endif  // PARSE_LL_LITERAL_HPP_INCLUDED

