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
Observer that prints the trace out to a std::ostream.
*/

#ifndef PARSE_LL_DEBUG_OSTREAM_OBSERVER_HPP_INCLUDED
#define PARSE_LL_DEBUG_OSTREAM_OBSERVER_HPP_INCLUDED

#include <iosfwd>
#include <iomanip>
// For std::cout, which is a default parameter for ostream_observer.
#include <iostream>

namespace parse_ll {

// Forward definition
template <class Range> class text_location_range;

class ostream_observer {
    std::ostream * stream;

    std::ostream & indent (int depth) const {
        for (int i = 0; i != depth; ++ i)
            if ((i+1) % 4)
                *stream << ' ';
            else
                *stream << '.';
        return *stream;
    }

    template <class Input> void
        print_input (Input start, Input const & finish) const {
        *stream << " \"";
        for (int i = 0; start != finish && i != 60; ++ i, start = drop (start))
        {
            char c = first (start);
            if (c >= 0x20 && c < 0x80)
                *stream << c;
            else {
                auto save_flags = stream->flags();
                *stream << '\\' << std::hex << std::showbase << unsigned (c);
                stream->flags (save_flags);
            }
        }
        *stream << "\"";
        if (start != finish)
            *stream << "...";
    }

    template <class Input> void
        describe_start_input (Input const &) const {}
    template <class Input> void
        describe_input (Input const & start, Input const & finish) const {
        print_input (start, finish);
    }

    template <class Input> void
        describe_start_input (text_location_range <Input> const & start) const {
        *stream << " ("
            << start.line() + 1 << "," << start.column() + 1 << ")";
    }
    template <class Input> void
        describe_consumed (text_location_range <Input> const & start,
            text_location_range <Input> const & finish) const {
        *stream << " ("
            //<< start.line() + 1 << "," << start.column() + 1 << " - "
            << finish.line() + 1 << "," << finish.column() + 1 << ")";
        print_input (start.underlying(), finish.underlying());
    }

public:
    ostream_observer (std::ostream & stream) : stream (&stream) {}

    ostream_observer() : stream (&std::cout) {}

    template <class Parser, class Input>
        void start_parse (int depth, Parser const & parser, Input const & input)
            const
    {
        indent (depth) << describe (parser);
        describe_start_input (input);
        *stream  << std::endl;
    }

    template <class Parser, class Input, class Outcome>
        void success (int depth, Parser const & parser, Input const & input,
            Input const & rest, Outcome const &) const
    {
        indent (depth) << describe (parser) << " successful";
        describe_consumed (input, rest);
        *stream << std::endl;
    }

    template <class Parser, class Input>
        void no_success (int depth, Parser const & parser, Input const & input)
            const
    {
        indent (depth) << describe (parser) << " failed";
        *stream << std::endl;
    }
};

} // namespace parse_ll

#endif  // PARSE_LL_DEBUG_OSTREAM_OBSERVER_HPP_INCLUDED

