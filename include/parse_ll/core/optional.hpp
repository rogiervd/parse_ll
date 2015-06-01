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
        template <class Policy, class SubParser, class Input> struct result {
            typedef typename std::decay <Input>::type bare_input_type;
            typedef typename detail::parser_outcome <Policy, SubParser,
                bare_input_type>::type sub_outcome_type;
            typedef typename detail::outcome_output <sub_outcome_type>::type
                sub_output_type;
            typedef typename boost::mpl::if_ <
                std::is_same <sub_output_type, void>,
                void, boost::optional <sub_output_type>>::type output_type;

            typedef successful <output_type, bare_input_type> type;
        };

        template <class Policy, class SubParser, class Input>
            typename result <Policy, SubParser, Input>::type
        operator() (Policy const & policy,
            optional_parser <SubParser> const & parser, Input && input) const
        {
            auto sub_outcome = parse_ll::parse (policy,
                parser.sub_parser, std::forward <Input> (input));
            if (::parse_ll::success (sub_outcome))
                return sub_outcome;
            else
                // Default-construct the output type, i.e. an empty
                // boost::optional <...>, or a void.
                return typename result <Policy, SubParser, Input>::type (input);
        }
    };

    template <> struct describe <optional_parser_tag> {
        template <class Parser> const char * operator() (Parser const &) const
        { return "optional"; }
    };
} // namespace operation

} // namespace parse_ll

#endif // PARSE_LL_BASE_REPEAT_HPP_INCLUDED

