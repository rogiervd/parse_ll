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
Define a parser that wraps a parser to check whether it behaves properly.

This code uses dynamic allocation as the main tool to try and confuse the
parser.
Problems therefore should easily be picked up by Valgrind or by using another
form of memory debugging.
*/

#ifndef PARSE_LL_TEST_HELPER_FUZZ_PARSER_HPP_INCLUDED
#define PARSE_LL_TEST_HELPER_FUZZ_PARSER_HPP_INCLUDED

#include <memory>
#include <type_traits>

#include "utility/returns.hpp"

#include "parse_ll/core/core.hpp"

template <class Parse, class SubParser, class Input> class fuzz_outcome;
template <class SubParser, class DerivedSubParser> struct fuzz_parser;

template <class Parser, class DerivedParser>
    inline auto fuzz_slicing (
        Parser const & parser, parse_ll::parser_base <DerivedParser> const &)
RETURNS (fuzz_parser <Parser, DerivedParser> (parser))

template <class SubParser>
    inline auto fuzz (SubParser const & sub_parser)
RETURNS (fuzz_slicing (sub_parser, sub_parser))

/**
Wrap a parser for debugging, to stress-test it.
Parsers can be copied, but have to stay in memory for as long as their
outcomes or outputs are around.
With the difference between SubParser and DerivedSubParser, slicing can be
checked.
*/
template <class SubParser, class DerivedSubParser> struct fuzz_parser
: public parse_ll::parser_base <fuzz_parser <SubParser, DerivedSubParser>>
{
    int check;
    static constexpr int initialised = 425798452;
    static constexpr int uninitialised = 354065413;

    void assert_initialised() const {
        assert (check == initialised);
    }

    std::shared_ptr <DerivedSubParser> sub_parser;
public:
    fuzz_parser (SubParser const & argument_sub_parser)
    : check (uninitialised)
    {
        // Copy parser
        std::unique_ptr <SubParser> initial_parser (
            new SubParser (argument_sub_parser));
        // Move parser, slicing if possible.
        sub_parser = std::shared_ptr <DerivedSubParser> (
            new DerivedSubParser (std::move (*initial_parser)));
        // Now sit still, because the parser is guaranteed to remain in memory.
        check = initialised;
    }

    ~fuzz_parser() {
        assert_initialised();
        check = uninitialised;
    }

    // Perform a deep copy
    fuzz_parser (fuzz_parser const & other) {
        other.assert_initialised();
        sub_parser = std::shared_ptr <DerivedSubParser> (
            new DerivedSubParser (*other.sub_parser));
        check = initialised;
    }

    // Perform a deep move
    fuzz_parser (fuzz_parser && other) {
        other.assert_initialised();
        sub_parser = std::shared_ptr <DerivedSubParser> (
            new DerivedSubParser (std::move (*other.sub_parser)));
        // Make sure to delete other.sub_parser.
        other.sub_parser.reset();
        check = initialised;
    }
};

/**
Wrap a parser outcome for debugging.
Outcomes are allowed to be copied and destructed at will.
That is, indeed, what fuzz_outcome intends to do.
*/
template <class Parse, class SubParser, class Input> class fuzz_outcome {
    int check;
    static constexpr int initialised = 987164613;
    static constexpr int uninitialised = 14687234;

    std::weak_ptr <SubParser> sub_parser;
    typedef typename parse_ll::detail::parser_outcome <Parse, SubParser, Input
        >::type sub_outcome_type;
    std::shared_ptr <sub_outcome_type> sub_outcome;

public:
    fuzz_outcome (Parse const & parse,
        std::shared_ptr <SubParser> sub_parser, Input const & input)
    : check (uninitialised), sub_parser (sub_parser) {
        std::unique_ptr <sub_outcome_type> temporary_outcome;
        {
            // The parse function is only guaranteed to exist as long as it is
            // being called...
            std::unique_ptr <Parse> temporary_parse (new Parse (parse));
            temporary_outcome = std::unique_ptr <sub_outcome_type> (
                new sub_outcome_type ((*temporary_parse) (*sub_parser, input)));
            // ... so destruct it immediately.
        }
        // Move
        sub_outcome = std::shared_ptr <sub_outcome_type> (new sub_outcome_type (
            std::move (*temporary_outcome)));
        check = initialised;
        // temporary_outcome is destructed.
    }

    ~fuzz_outcome() {
        assert_invariants();
        check = uninitialised;
    }

    // Perform a deep copy
    fuzz_outcome (fuzz_outcome const & other)
    : check (uninitialised), sub_parser (other.sub_parser)
    {
        other.assert_invariants();
        sub_outcome = std::shared_ptr <sub_outcome_type> (
            new sub_outcome_type (*other.sub_outcome));
        check = initialised;
    }

    // Perform a deep copy
    fuzz_outcome (fuzz_outcome && other)
    : check (uninitialised), sub_parser (other.sub_parser)
    {
        other.assert_invariants();
        sub_outcome = std::shared_ptr <sub_outcome_type> (
            new sub_outcome_type (std::move (*other.sub_outcome)));
        // Make sure to delete other.sub_outcome.
        other.sub_outcome.reset();
        check = initialised;
    }

    // Perform a deep copy
    fuzz_outcome & operator = (fuzz_outcome const & other)
    {
        other.assert_invariants();
        sub_parser = other.sub_parser;
        sub_outcome = std::shared_ptr <sub_outcome_type> (
            new sub_outcome_type (*other.sub_outcome));
        return *this;
    }
    // Perform a deep copy
    fuzz_outcome & operator = (fuzz_outcome && other)
    {
        other.assert_invariants();
        sub_parser = other.sub_parser;
        sub_outcome = std::shared_ptr <sub_outcome_type> (
            new sub_outcome_type (std::move (*other.sub_outcome)));
        // Make sure to delete other.sub_outcome.
        other.sub_outcome.reset();
        return *this;
    }

    void assert_invariants() const {
        assert (check == initialised);
        // The parser must still exist.
        std::shared_ptr <SubParser> current_sub_parser = sub_parser.lock();
        assert (current_sub_parser);
    }

    /// Make a copy of sub_outcome.
    std::unique_ptr <sub_outcome_type> sub_outcome_copy() const {
        std::unique_ptr <sub_outcome_type> copy (
            new sub_outcome_type (*sub_outcome));
        return std::move (copy);
    }
};

namespace parse_ll { namespace operation {

    template <class Parse, class SubParser, class DerivedSubParser, class Input>
        struct parse <Parse, fuzz_parser <SubParser, DerivedSubParser>, Input>
    {
        fuzz_outcome <Parse, DerivedSubParser, Input> operator() (
            Parse const & parse,
            fuzz_parser <SubParser, DerivedSubParser> const & parser,
            Input const & input) const
        {
            parser.assert_initialised();
            return fuzz_outcome <Parse, DerivedSubParser, Input> (parse,
                parser.sub_parser, input);
        }
    };

    template <class Parse, class SubParser, class Input>
        struct success <fuzz_outcome <Parse, SubParser, Input>>
    {
        bool operator() (fuzz_outcome <Parse, SubParser, Input> const & outcome)
            const
        {
            outcome.assert_invariants();
            return ::parse_ll::success (*outcome.sub_outcome_copy());
        }
    };

    template <class Parse, class SubParser, class Input>
        struct output <fuzz_outcome <Parse, SubParser, Input>>
    {
        auto operator() (fuzz_outcome <Parse, SubParser, Input> const & outcome)
            const -> decltype (::parse_ll::output (*outcome.sub_outcome_copy()))
        {
            outcome.assert_invariants();
            return ::parse_ll::output (*outcome.sub_outcome_copy());
        }
    };

    template <class Parse, class SubParser, class Input>
        struct rest <fuzz_outcome <Parse, SubParser, Input>>
    {
        Input operator() (fuzz_outcome <Parse, SubParser, Input> const &
            outcome) const
        {
            outcome.assert_invariants();
            return ::parse_ll::rest (*outcome.sub_outcome_copy());
        }
    };

}} // namespace parse_ll::operation


#endif  // PARSE_LL_TEST_HELPER_FUZZ_PARSER_HPP_INCLUDED

