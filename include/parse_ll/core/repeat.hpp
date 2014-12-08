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
Define a repeat parser, which parses the same thing a number of times.
It outputs a sequence of sub-parser outputs.
*/

#ifndef PARSE_LL_BASE_REPEAT_HPP_INCLUDED
#define PARSE_LL_BASE_REPEAT_HPP_INCLUDED

#include <cassert>

#include <boost/optional.hpp>

#include "range/core.hpp"

#include "core.hpp"

#include <boost/mpl/if.hpp>
#include <type_traits>

namespace parse_ll {

/**
Parser that repeats it sub-parser a number of times, where the number can be
constrained.

Between each sub-parser the skip parser is used, but not before or after the
repeat parser.
*/
template <class SubParser> struct repeat_parser
    : public parser_base <repeat_parser <SubParser> >
{
    SubParser sub_parser;
    int minimum, maximum;
public:
    repeat_parser (SubParser const & sub_parser, int minimum, int maximum)
    : sub_parser (sub_parser), minimum (minimum), maximum (maximum) {}
};

struct repeat_parser_tag;
template <class SubParser> struct decayed_parser_tag <repeat_parser <SubParser>>
{ typedef repeat_parser_tag type; };

class repeat_parser_maker_bounds {
    int minimum, maximum;
public:
    repeat_parser_maker_bounds (int minimum, int maximum)
    : minimum (minimum), maximum (maximum) {}

    template <class SubParser>
        repeat_parser <SubParser>
            operator[] (SubParser const & sub_parser) const
    { return repeat_parser <SubParser> (sub_parser, minimum, maximum); }
};

struct repeat_parser_maker {
    template <class SubParser>
        repeat_parser <SubParser>
            operator[] (SubParser const & sub_parser) const
    { return repeat_parser <SubParser> (sub_parser, 0, -1); }

    repeat_parser_maker_bounds operator() (int count) const
    { return repeat_parser_maker_bounds (count, count); }

    repeat_parser_maker_bounds operator() (int minimum, int maximum) const
    { return repeat_parser_maker_bounds (minimum, maximum); }

    repeat_parser_maker_bounds at_least (int minimum) const
    { return repeat_parser_maker_bounds (minimum, -1); }

    repeat_parser_maker_bounds at_most (int maximum) const
    { return repeat_parser_maker_bounds (0, maximum); }
};

static const auto repeat = repeat_parser_maker();

/**
Implementations that are optimal in different scenarios.
For now, there is only one implementation.

\todo
One implementation would be to cache the remaining input range.
However, this must be shared between repeat_outcome(s) and repeat_output.
repeat_output should set it once it hits the final parse.
This should probably be done in a shared_ptr <optional <Input>>.
Now that's some extra investment, and it may make sense only for big parsers.

\todo
The reason why it is impossible to ask output() for the rest of the input range
after it's been exhausted, is that actors will be passed the input range and
never give it back.
If this is a common occurrence, then it may be possible to specialise
transform_parse <repeat_outcome <...>> for this case?

*/
enum class repeat_type { lazy };

template <class Policy, class SubParser, class Input,
    repeat_type Implementation> struct repeat_outcome;
template <class Policy, class SubParser, class Input,
    repeat_type Implementation> struct repeat_output;
struct repeat_output_range_tag;

namespace operation {

    template <> struct parse <repeat_parser_tag> {
        template <class Policy, class SubParser, class Input>
            repeat_outcome <Policy, SubParser, Input, repeat_type::lazy>
        operator() (Policy const & policy,
            repeat_parser <SubParser> const & parser, Input const & input) const
        {
            return repeat_outcome <Policy, SubParser, Input, repeat_type::lazy>
                (policy, parser, input);
        }
    };

    template <> struct describe <repeat_parser_tag> {
        template <class Parser> const char * operator() (Parser const &) const
        { return "repeat"; }
    };

} // namespace operation

/**** Implementation: lazy ****/

/**
With minimum and maximum lengths, it may be best to cache the sub-parses.
This would require the output to be a concatenation of the output of the cached
sub-parses plus the on-the-fly sub-parses.
For now, the constructor checks whether the parse will succeed, i.e., whether
the minimum number of sub-parses succeed.
When actually producing the output, the sub-parses are regenerated.
This may be faster in one case and slower in another.

\todo
It would be great if this would cause a static assertion failure in cases where
the sub-parser always succeeds.
Even I got caught out by this in writing test cases.
*/
template <class Policy, class SubParser, class Input>
    struct repeat_outcome <Policy, SubParser, Input, repeat_type::lazy>
{
    Policy policy;
    repeat_parser <SubParser> const * parser;
    Input input;
public:
    repeat_outcome (Policy const & policy,
        repeat_parser <SubParser> const & parser, Input const & input)
    : policy (policy), parser (&parser), input (input) {}
};

namespace operation {

    template <class Policy, class SubParser, class Input>
        struct success <repeat_outcome <
            Policy, SubParser, Input, repeat_type::lazy>>
    {
        bool operator() (
            repeat_outcome <Policy, SubParser, Input, repeat_type::lazy>
            const & outcome) const
        {
            // Check whether the minimum number of parses of the sub-parser
            // can be obtained.
            Input current = outcome.input;
            for (int count = 0; count < outcome.parser->minimum; ++ count) {
                if (count != 0)
                    current = parse_ll::skip_over (
                        outcome.policy.skip_parser(), current);
                auto sub_outcome = parse_ll::parse (
                    outcome.policy, outcome.parser->sub_parser, current);
                if (!::parse_ll::success (sub_outcome))
                    return false;
                current = parse_ll::rest (sub_outcome);
            }
            return true;
        }
    };

    template <class Policy, class SubParser, class Input>
        struct output <repeat_outcome <
            Policy, SubParser, Input, repeat_type::lazy>>
    {
        typedef typename parse_ll::detail::parser_output <
            Policy, SubParser, Input>::type sub_output_type;

        /**
        Select the output type.
        If the sub-parser outputs void, then the repeat_parser also outputs
        void.
        */
        typedef typename boost::mpl::if_ <std::is_same <sub_output_type, void>,
                void,
                repeat_output <Policy, SubParser, Input, repeat_type::lazy>
            >::type output_type;

        // If output_type is void, this never gets instantiated.
        output_type operator() (
            repeat_outcome <Policy, SubParser, Input, repeat_type::lazy>
                const & outcome) const
        {
            assert (::parse_ll::success (outcome));
            return repeat_output <Policy, SubParser, Input, repeat_type::lazy>
                (outcome.policy,
                    *outcome.parser, outcome.parser->maximum, outcome.input);
        }
    };

    template <class Policy, class SubParser, class Input>
        struct rest <repeat_outcome <
            Policy, SubParser, Input, repeat_type::lazy>>
    {
        Input operator() (
            repeat_outcome <Policy, SubParser, Input, repeat_type::lazy>
                const & outcome) const
        {
            assert (::parse_ll::success (outcome));
            // Run the sub_parser through the input.
            Input current = outcome.input;
            for (int count = 0; count != outcome.parser->maximum; ++ count) {
                auto sub_outcome = parse_ll::parse (
                    outcome.policy, outcome.parser->sub_parser,
                    // Only skip in between elements, not before.
                    (count == 0) ? current : parse_ll::skip_over (
                        outcome.policy.skip_parser(), current));
                // If the parser has failed, current is still at rest() applied
                // to the sub-parser that last succeeded: the skip parser has
                // not been applied.
                if (! ::parse_ll::success (sub_outcome)) {
                    assert (count >= outcome.parser->minimum);
                    return current;
                }
                current = ::parse_ll::rest (sub_outcome);
            }
            return current;
        }
    };

} // namespace operation

/**
Lazy implementation of the output of a repeat parser, as a range.
The implementation essentially wraps the outcome of the sub-parser, in
sub_outcome.
The range is empty if sub_outcome has not succeeded (or if maximum==0).
The first element is the output of sub_outcome.
drop() uses the rest of sub_outcome.

\todo When maximum==0, the sub_outcome does not even have to be instantiated.
*/
template <class Policy, class SubParser, class Input>
    struct repeat_output <Policy, SubParser, Input, repeat_type::lazy>
{
    Policy policy;
    // If this were a reference, the class could not be copy-assigned.
    repeat_parser <SubParser> const * parser;
    int maximum;
    typedef typename detail::parser_outcome <Policy, SubParser, Input>::type
        sub_outcome_type;
    sub_outcome_type sub_outcome;
public:
    repeat_output (Policy const & policy,
        repeat_parser <SubParser> const & parser,
        int maximum, Input const & input)
    : policy (policy), parser (&parser), maximum (maximum),
        sub_outcome (parse_ll::parse (policy, parser.sub_parser, input)) {}
};

} // namespace parse_ll

namespace range {

template <class Policy, class SubParser, class Input,
    parse_ll::repeat_type Implementation>
struct tag_of_qualified <parse_ll::repeat_output <
    Policy, SubParser, Input, Implementation>>
{ typedef parse_ll::repeat_output_range_tag type; };

namespace operation {

    template<>
        struct empty <parse_ll::repeat_output_range_tag, direction::front>
    {
        template <class Policy, class SubParser, class Input>
            bool operator() (direction::front,
                parse_ll::repeat_output <Policy, SubParser, Input,
                    parse_ll::repeat_type::lazy> const & o) const
        {
            if (o.maximum != 0 && ::parse_ll::success (o.sub_outcome))
                return false;
            else
                return true;
        }
    };

    template<>
        struct first <parse_ll::repeat_output_range_tag, direction::front>
    {
        template <class Policy, class SubParser, class Input>
            auto operator() (direction::front,
                parse_ll::repeat_output <Policy, SubParser, Input,
                    ::parse_ll::repeat_type::lazy> const & o) const
        -> decltype (::parse_ll::output (o.sub_outcome))
        {
            assert (!::range::empty (o));
            return ::parse_ll::output (o.sub_outcome);
        }
    };

    template<>
        struct drop_one <parse_ll::repeat_output_range_tag, direction::front>
    {
        template <class Increment, class Output>
            Output operator() (direction::front, Increment, Output const & o)
            const
        {
            assert (!::range::empty (o));
            auto next_range = parse_ll::skip_over (
                o.policy.skip_parser(), ::parse_ll::rest (o.sub_outcome));
            return Output (o.policy, *o.parser, o.maximum - 1, next_range);
        }
    };

}} // namespace range::operation

#endif // PARSE_LL_BASE_REPEAT_HPP_INCLUDED

