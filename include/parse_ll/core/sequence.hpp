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
Define a sequence parser, which parses two inputs after one another
*/

#ifndef PARSE_LL_SEQUENCE_HPP_INCLUDED
#define PARSE_LL_SEQUENCE_HPP_INCLUDED

#include "fwd.hpp"
#include "core.hpp"
#include "error.hpp"

// For std::tuple
#include <utility>

#include <boost/optional.hpp>
#include <boost/utility/typed_in_place_factory.hpp>

namespace parse_ll {

template <bool expect, class Parser1, class Parser2> struct sequence_parser
: parser_base <sequence_parser <expect, Parser1, Parser2> >
{
    Parser1 parser_1;
    Parser2 parser_2;
public:
    sequence_parser (Parser1 const & parser_1, Parser2 const & parser_2)
    : parser_1 (parser_1), parser_2 (parser_2) {}
};

struct sequence_parser_tag;
template <bool expect, class Parser1, class Parser2>
    struct decayed_parser_tag <sequence_parser <expect, Parser1, Parser2>>
{ typedef sequence_parser_tag type; };

template <bool expect, class Policy, class Parser1, class Parser2, class Input,
    class Output1 = typename detail::parser_output <Policy, Parser1, Input
        >::type,
    class Output2 = typename detail::parser_output <Policy, Parser2, Input
        >::type>
struct sequence_outcome;

/**
Sequence of two consecutive parses.
If the first parser is a sequence parser, the output of the second parser is
appended to the output of the first one.
This can therefore be seen as a parser that uses recursion to implement
variable-length sequences.

The expect parser variant of the sequence parser has "expect = 1".
It is like a sequence parser but does not retain the left parse, because
backtracking is impossible once the left parse has succeeded.
The second parser is expected to succeed if the first does, otherwise an
exception is thrown.
success() can therefore short-circuit.

\todo
With a proper range library, it should probably be implemented with a variadic
sequence of parsers.

\note
All outcomes are currently kept in memory.
However, outcomes may be large-ish, and include redundant input ranges.
It may be possible to keep a reduced version of the parses, that is asserted
to have been successful and does not keep the input. Laborious.

\todo
The lazy version of sequence_parser should roughly correspond to the one for
repeat_parser.
It may even use similar strategies for caching rest().

\todo
What should be the model for a > b >> c ?
Once a has succeeded, both b and c must succeed.
One of three options must be chosen:
1.  > and >> are separate, as currently implemented.
    If an error occurs in c, the position reported will be at b.
2.  > infects >>.
    This is essentially a change from what the writer of the parser intended,
    but the only difference, really, is where the error is reported.
    The great advantage would be that the input range for b will not be
    retained once b has been parsed.
3.  >> following > will return a different error type, which contains both
    input ranges

\todo
When Rime gets used, it will be possible to automatically turn "expect" on
if parser_2.success() is always true.

\todo It would be good to get rid of Output1 and Output2, which might lead to
horribly long compiler errors.
However, in the recursive version of this, this makes operation::output very
complicated.
Postpone until the sequential version.
*/
template <bool expect, class Policy, class Parser1, class Parser2, class Input,
    class Output1, class Output2>
struct sequence_outcome
{
    typedef typename detail::parser_outcome <Policy, Parser1, Input>::type
        outcome_1_type;
    typedef typename detail::parser_outcome <Policy, Parser2, Input>::type
        outcome_2_type;
    outcome_1_type outcome_1;
    // If success (outcome_1):
    boost::optional <outcome_2_type> outcome_2;
    // If !success (outcome_1):
    boost::optional <Input> input;
public:
    sequence_outcome (Policy const & policy, Parser1 const & parser_1,
        Parser2 const & parser_2, Input const & input)
    : outcome_1 (parse (policy, parser_1, input)) {
        if (success (outcome_1)) {
            // The in-place factory makes sure that operator= is not needed.
            outcome_2 = boost::in_place <outcome_2_type, outcome_2_type> (
                parse (policy, parser_2,
                    skip_over (policy.skip_parser(), rest (outcome_1))));
            // For an expect parser, outcome_2 must succeed if outcome_1 does.
            if (expect && !success (*outcome_2))
                // Otherwise, construction fails.
                throw error() << error_at <Input> (rest (outcome_1));
        } else
            this->input = input;
    }
};

namespace operation {
    template <> struct parse <sequence_parser_tag> {
        template <class Policy,
                bool expect, class Parser1, class Parser2, class Input>
        sequence_outcome <expect, Policy, Parser1, Parser2, Input>
            operator() (Policy const & policy,
                sequence_parser <expect, Parser1, Parser2>
                const & parser, Input const & input) const
        {
            return sequence_outcome <expect, Policy, Parser1, Parser2, Input> (
                policy, parser.parser_1, parser.parser_2, input);
        }
    };

    template <> struct describe <sequence_parser_tag> {
        template <class Parser> const char * operator() (Parser const &) const
        { return "sequence"; }
    };

    template <bool except, class Policy,
            class Parser1, class Parser2, class Input>
        struct success <
            sequence_outcome <except, Policy, Parser1, Parser2, Input>>
    {
        bool operator() (
            sequence_outcome <false, Policy, Parser1, Parser2, Input>
            const & outcome) const
        {
            return ::parse_ll::success (outcome.outcome_1)
                && ::parse_ll::success (*outcome.outcome_2);
        }
        bool operator() (sequence_outcome <true, Policy, Parser1, Parser2, Input
            > const & outcome) const
        {
            // If the first parser succeeds, the next must do too.
            return ::parse_ll::success (outcome.outcome_1);
        }
    };

    // something1 + someting2 -> tuple <something1, something2>
    template <bool except, class Policy, class Parser1, class Parser2,
            class Input, class Output1, class Output2>
        struct output <sequence_outcome <except, Policy, Parser1, Parser2,
            Input, Output1, Output2>>
    {
        std::tuple <Output1, Output2>
            operator() (sequence_outcome <except, Policy, Parser1, Parser2,
                Input> const & outcome) const
        {
            return std::tuple <Output1, Output2> (
                ::parse_ll::output (outcome.outcome_1),
                ::parse_ll::output (*outcome.outcome_2));
        }
    };
    //*** Special case type 1: void
    // void + void -> void
    template <bool except, class Policy, class Parser1, class Parser2,
            class Input>
        struct output <sequence_outcome <
            except, Policy, Parser1, Parser2, Input, void, void>>
    {
        // Never executed because it is short-circuited globally.
        void operator() (sequence_outcome <except, Policy, Parser1, Parser2,
                Input> const & outcome) const;
    };
    // something + void -> tuple <something>
    template <bool except, class Policy, class Parser1, class Parser2,
            class Input, class Output1>
        struct output <sequence_outcome <
            except, Policy, Parser1, Parser2, Input, Output1, void>>
    {
        std::tuple <Output1>
            operator() (sequence_outcome <except, Policy, Parser1, Parser2,
                Input> const & outcome) const
        {
            return std::tuple <Output1> (
                ::parse_ll::output (outcome.outcome_1));
        }
    };
    // void + something -> tuple <something>
    template <bool except, class Policy, class Parser1, class Parser2,
            class Input, class Output2>
        struct output <sequence_outcome <
            except, Policy, Parser1, Parser2, Input,void, Output2>>
    {
        std::tuple <Output2>
            operator() (sequence_outcome <except, Policy, Parser1, Parser2,
                Input> const & outcome) const
        {
            return std::tuple <Output2> (
                ::parse_ll::output (*outcome.outcome_2));
        }
    };
    //*** Special case type 2: Parser1 is a sequence_parser.
    // tuple <something1...> (from sequence_parser) + something2
    // -> tuple <something1..., something2>
    template <bool except, class Policy, class Parser1, class Parser2,
            class Input, class ... Output1, class Output2>
        struct output <sequence_outcome <except, Policy, Parser1, Parser2,
                Input, std::tuple <Output1...>, Output2>>
    {
        std::tuple <Output1 ..., Output2>
            operator() (sequence_outcome <except, Policy, Parser1, Parser2,
                Input> const & outcome) const
        {
            return std::tuple_cat (::parse_ll::output (outcome.outcome_1),
                std::tuple <Output2> (::parse_ll::output (*outcome.outcome_2)));
        }
    };
    // tuple <something...> (from sequence_parser) + void
    // -> tuple <something...>
    template <bool except, class Policy, bool sub_except, class Parser11,
            class Parser12, class Parser2, class Input, class ... Output1>
        struct output <sequence_outcome <except, Policy,
                sequence_parser <sub_except, Parser11, Parser12>,
                Parser2, Input, std::tuple <Output1...>, void>>
    {
        template <class Parser1>
        std::tuple <Output1...>
            operator() (sequence_outcome <except, Policy, Parser1, Parser2,
                Input> const & outcome) const
        { return ::parse_ll::output (outcome.outcome_1); }
    };

    template <bool except, class Policy, class Parser1, class Parser2,
            class Input>
        struct rest <sequence_outcome <except, Policy, Parser1, Parser2, Input>>
    {
        Input operator() (sequence_outcome <except, Policy, Parser1, Parser2,
            Input> const & outcome) const
        { return ::parse_ll::rest (*outcome.outcome_2); }
    };

} // namespace operation

} // namespace parse_ll

#endif  // PARSE_LL_SEQUENCE_HPP_INCLUDED

