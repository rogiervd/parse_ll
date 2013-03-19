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
Define an optional parser, which parses something zero or one time.
*/

#ifndef PARSE_LL_BASE_OPTIONAL_HPP_INCLUDED
#define PARSE_LL_BASE_OPTIONAL_HPP_INCLUDED

#include <cassert>
#include <type_traits>
#include <boost/mpl/if.hpp>
#include <boost/optional.hpp>

#include "fwd.hpp"
#include "core.hpp"

namespace parse_ll {

/**
Parser that wraps a parser so that it always succeeds, and outputs a
boost::optional<>.
If the sub-parser had void as its output, this parser will also output a void.
This is consistent with the objective of translating the input to an output
type as opposed to returning some information about whether a parser matched
or not.

\todo Currently, parse() returns an object of class successful<>.
This implementation is simple.
However, if the underlying parser is successful, output() is always called.
This can be prevented by introducing a class optional_outcome<>.
It would keep a copy of the sub-outcome and call it when necessary.
It that does require explicit handling of void.
Also, either the input range should be saved, but preferably only if the
underlying parser is unsuccessful, because otherwise it'll hold the rest of
the input anyway.
So you'd imagine a variant<sub_outcome, Input> (and maybe void?) but even that
duplicates information: sub_outcome contains information about whether it was
successful, whereas in this context the variant already indicates that.
So maybe the current implementation should be used if the sub_parser's outcome
is explicit_outcome and the new implementation otherwise?
Or outcomes could be asserted successful and shed some of their weight...?
*/
template <class SubParser> struct optional_parser
    : public parser_base <optional_parser <SubParser> >
{
    SubParser sub_parser;
public:
    optional_parser (SubParser const & sub_parser) : sub_parser (sub_parser) {}
};

struct optional_parser_tag;

template <class SubParser>
    struct decayed_parser_tag <optional_parser <SubParser>>
{ typedef optional_parser_tag type; };

namespace operation {

    template <> struct parse <optional_parser_tag> {
        template <class Parse, class SubParser, class Input> struct result {
            typedef typename std::decay <Input>::type bare_input_type;
            typedef typename detail::parser_outcome <Parse, SubParser,
                bare_input_type>::type sub_outcome_type;
            typedef typename detail::outcome_output <sub_outcome_type>::type
                sub_output_type;
            typedef typename boost::mpl::if_ <
                std::is_same <sub_output_type, void>,
                void, boost::optional <sub_output_type>>::type output_type;

            typedef successful <output_type, bare_input_type> type;
        };

        template <class Parse, class SubParser, class Input>
            typename result <Parse, SubParser, Input>::type
        operator() (Parse const & parse,
            optional_parser <SubParser> const & parser, Input && input) const
        {
            auto sub_outcome =
                parse (parser.sub_parser, std::forward <Input> (input));
            if (::parse_ll::success (sub_outcome))
                return sub_outcome;
            else
                // Default-construct the output type, i.e. an empty
                // boost::optional <...>, or a void.
                return typename result <Parse, SubParser, Input>::type (input);
        }
    };

    template <> struct describe <optional_parser_tag> {
        template <class Parser> const char * operator() (Parser const &) const
        { return "optional"; }
    };
} // namespace operation

} // namespace parse_ll

#endif // PARSE_LL_BASE_REPEAT_HPP_INCLUDED

