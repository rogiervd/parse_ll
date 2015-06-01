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

/**
Observer that prints the trace out to a std::ostream.
*/

#ifndef PARSE_LL_DEBUG_OSTREAM_OBSERVER_HPP_INCLUDED
#define PARSE_LL_DEBUG_OSTREAM_OBSERVER_HPP_INCLUDED

#include <iosfwd>
#include <iomanip>
// For std::cout, which is a default parameter for ostream_observer.
#include <iostream>

#include "parse_ll/support/text_location_range.hpp"

namespace parse_ll {

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

    template <class Input> void print_input (Input start, Input const & finish)
        const
    {
        *stream << " \"";
        for (int i = 0; start != finish && i != 60;
            ++ i, start = range::drop (start))
        {
            auto c = range::first (start);
            if (c >= 0x20 && int (c) < 0x80)
                *stream << c;
            else {
                auto save_flags = stream->flags();
                *stream << '\\' << std::hex << std::showbase
                    << unsigned ((unsigned char) c);
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
        describe_start_input (range::text_location_range <Input> const & start)
        const
    {
        *stream << " ("
            << start.line() + 1 << "," << start.column() + 1 << ")";
    }
    template <class Input> void
        describe_consumed (range::text_location_range <Input> const & start,
            range::text_location_range <Input> const & finish) const {
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

