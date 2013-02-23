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
Define the skip directive, which makes the sub-parser insert a skip parser
between each component parser.

This could use detail/directive.hpp for changing the parse.
However, skip_pad needs to to more, and it makes sense to share code between it
and skip/
*/

#ifndef PARSE_LL_BASE_SKIP_HPP_INCLUDED
#define PARSE_LL_BASE_SKIP_HPP_INCLUDED

#include "core.hpp"

namespace parse_ll {

template <class SkipParser, bool pad>
    class skip_inside_directive;
template <class SubParser, class SkipParser, bool pad>
    struct skip_inside;

/**
Create a parser that uses a specific skip parser inside the sub-parser.
This is used as, for example, skip (whitespace) [sub_parser].

\internal
The expression skip (whitespace) constructs a skip_inside_directive.
Calling its operator[] produces a skip_inside.
skip_inside then changes the parse policy for its sub-parser.
*/
template <class SkipParser> auto skip (SkipParser const & skip_parser)
    RETURNS (skip_inside_directive <SkipParser, false> (skip_parser));
/**
Create a parser that uses a specific skip parser before, inside, and after the
sub-parser.
This is used as, for example, skip_pad (whitespace) [sub_parser].
*/
template <class SkipParser> auto skip_pad (SkipParser const & skip_parser)
    RETURNS (skip_inside_directive <SkipParser, true> (skip_parser));

namespace parse_policy {

    template <class SkipParser, class OriginalPolicy> struct skip_policy;

    /**
    Parse function that uses a different skip parser that the original parser
    does.

    \todo
    no_skip currently tries to optimise cases where it wraps policies that only
    change the skip parser. Is that possible here too?
    */
    template <class SkipParser, class OriginalPolicy> struct skip_policy
    : public OriginalPolicy
    {
        SkipParser skip_parser;
    public:
        skip_policy (SkipParser const & skip_parser,
            OriginalPolicy const & original_policy)
        : OriginalPolicy (original_policy), skip_parser (skip_parser) {}

        template <class Input> Input skip (Input const & input) const {
            auto outcome = ::parse_ll::parse (skip_parser, input);
            if (::parse_ll::success (outcome))
                return ::parse_ll::rest (outcome);
            else
                return input;
        }

        OriginalPolicy const & original_policy() const { return *this; }
    };

} // namespace parse_policy

template <class SkipParser, bool pad> class skip_inside_directive {
    SkipParser skip_parser;
public:
    explicit skip_inside_directive (SkipParser const & skip_parser)
    : skip_parser (skip_parser) {}

    template <class SubParser> skip_inside <SubParser, SkipParser, pad>
        operator[] (SubParser const & sub_parser) const {
        return skip_inside <SubParser, SkipParser, pad> (
            sub_parser, skip_parser);
    }
};

/**
Parser that replaces the skip parser active outside this parser, by skip_parser.
Normally produced by an expression like skip [skip_parser] (sub_parser).
*/
template <class SubParser, class SkipParser, bool pad> struct skip_inside
: parser_base <skip_inside <SubParser, SkipParser, pad>>
{
    SubParser sub_parser;
    SkipParser skip_parser;
public:
    skip_inside (SubParser const & sub_parser, SkipParser const & skip_parser)
    : sub_parser (sub_parser), skip_parser (skip_parser) {}
};

/**
Outcome that wraps the sub_parser's outcome so that the skip parser is used
before sub_parser (during the construction of this) and after sub_parser
(by rest()).
*/
template <class Parse, class SubParser, class Input>
    struct skip_inside_pad_outcome
{
    Parse parse;
    typedef typename detail::parser_outcome <Parse, SubParser, Input>::type
        outcome_type;
    outcome_type outcome;
public:
    skip_inside_pad_outcome (Parse const & parse, SubParser const & sub_parser,
        Input const & input)
    : parse (parse), outcome (parse (sub_parser, parse.skip (input))) {}
};

namespace operation {
    /**
    The handling of the normal skip() [..] is different from the handling
    of skip_pad [..].
    skip_pad() wraps the outcome, whereas skip() can get away with just
    calling a newly constructed skip_policy instead of original_parser.
    */
    template <class OriginalPolicy,
        class SubParser, class SkipParser, bool pad, class Input>
    struct parse <callable::parse <OriginalPolicy>,
        skip_inside <SubParser, SkipParser, pad>, Input>
    {
        typedef callable::parse <parse_policy::skip_policy <
            SkipParser, OriginalPolicy>> new_parse_type;

        // No padding.
        auto operator() (
            callable::parse <OriginalPolicy> const & original_parse,
            skip_inside <SubParser, SkipParser, false> const &
                parser, Input const & input) const
        RETURNS (
            // Produce new parse function that wraps original_parse.
            (new_parse_type (parser.skip_parser, original_parse.policy()))
            // Call it.
                (parser.sub_parser, input));

        // With padding: use skip_inside_pad_outcome.
        auto operator() (
            callable::parse <OriginalPolicy> const & original_parse,
            skip_inside <SubParser, SkipParser, true> const &
                parser, Input const & input) const
        RETURNS (skip_inside_pad_outcome <new_parse_type, SubParser, Input> (
            new_parse_type (parser.skip_parser, original_parse.policy()),
            parser.sub_parser, input));
    };

    template <class SubParser, class SkipParser, bool pad>
        struct describe <skip_inside <SubParser, SkipParser, pad>> {
        const char * operator() (skip_inside <SubParser, SkipParser, pad>
            const &) const
        { return "(change skip parser used inside)"; }
    };

    // Handling skip_inside_pad_outcome:
    // success() propagates from the internal parser.
    template <class Parse, class SubParser, class Input>
        struct success <skip_inside_pad_outcome <Parse, SubParser, Input>>
    {
        bool operator() (skip_inside_pad_outcome <Parse, SubParser, Input>
            const & outcome) const
        { return ::parse_ll::success (outcome.outcome); }
    };

    // output() propagates from the internal parser.
    template <class Parse, class SubParser, class Input>
        struct output <skip_inside_pad_outcome <Parse, SubParser, Input>>
    {
        auto operator() (skip_inside_pad_outcome <Parse, SubParser, Input>
            const & outcome) const
        RETURNS (::parse_ll::output (outcome.outcome));
    };

    // rest() takes rest (outcome.outcome) and applies the skip parser to it
    // again.
    template <class Parse, class SubParser, class Input>
        struct rest <skip_inside_pad_outcome <Parse, SubParser, Input>>
    {
        Input operator() (skip_inside_pad_outcome <Parse, SubParser, Input>
            const & outcome) const
        { return outcome.parse.skip (::parse_ll::rest (outcome.outcome)); }
    };

} // namespace operation

} // namespace parse_ll

#endif  // PARSE_LL_BASE_SKIP_HPP_INCLUDED

