// SPDX-FileCopyrightText: Copyright 2012 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

/** \file
Define a parser that accepts a literal and does not output anything.
*/

#ifndef PARSE_LL_LITERAL_HPP_INCLUDED
#define PARSE_LL_LITERAL_HPP_INCLUDED

#include <string>

#include "range/core.hpp"

#include "core.hpp"
#include "fwd.hpp"
#include "outcome/explicit.hpp"
#include "outcome/failed.hpp"

namespace parse_ll {

/**
Literal must be a range.
It can only be a view if the underlying range is guaranteed to stay in memory
throughout the lifetime of the literal_parser object.
*/
template <class Literal> struct literal_parser
: public parser_base<literal_parser<Literal>>
{
    Literal literal;

public:
    literal_parser(Literal const & literal) : literal(literal) {}
};

struct literal_parser_tag;

template <class Literal> struct decayed_parser_tag<literal_parser<Literal>>
{
    typedef literal_parser_tag type;
};

inline literal_parser<std::string> literal(char c)
{
    return literal_parser<std::string>(std::string(1, c));
}

inline literal_parser<std::string> literal(std::string const & s)
{
    return literal_parser<std::string>(s);
}

namespace operation {

    template <> struct parse<literal_parser_tag>
    {
        template <class Policy, class Literal, class Input>
        explicit_outcome<void, Input> operator()(
            Policy const &, literal_parser<Literal> const & parser,
            Input input) const
        {
            using ::range::drop;
            using ::range::empty;
            using ::range::first;
            auto literal = ::range::view(parser.literal);
            while (!empty(literal) && !empty(input)) {
                if (!(first(literal) == first(input))) {
                    return failed();
                }
                literal = drop(literal);
                input = drop(input);
            }
            if (empty(literal)) {
                // We went through the whole literal: success.
                return explicit_outcome<void, Input>(input);
            } else {
                return failed();
            }
        }
    };

    template <> struct describe<literal_parser_tag>
    {
        template <class Parser> char const * operator()(Parser const &) const
        {
            return "literal";
        }
    };

}  // namespace operation

}  // namespace parse_ll

#endif  // PARSE_LL_LITERAL_HPP_INCLUDED
