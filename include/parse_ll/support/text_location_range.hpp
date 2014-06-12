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
Define a range that remembers its position in a text file.
*/

#ifndef PARSE_LL_TEXT_LOCATION_RANGE_HPP_INCLUDED
#define PARSE_LL_TEXT_LOCATION_RANGE_HPP_INCLUDED

#include "utility/returns.hpp"

#include "range/core.hpp"

namespace range {

template <class Range> class text_location_range;
struct text_location_range_tag;

template <class Range> struct tag_of_bare <text_location_range <Range>>
{ typedef text_location_range_tag type; };

template <class Range> class text_location_range {
public:
    static const std::size_t tab_size = 4;

    Range underlying_;
    std::size_t line_, column_;
    bool last_carriage_return;

    text_location_range (Range && range,
        std::size_t line_, std::size_t column_, bool last_carriage_return)
    : underlying_ (std::forward <Range> (range)), line_ (line_),
        column_ (column_), last_carriage_return (last_carriage_return) {}

public:
    text_location_range (Range const & underlying_)
    : underlying_ (underlying_),
        line_ (0), column_ (0), last_carriage_return (false) {}

    bool operator == (text_location_range const & other) const
    { return underlying_ == other.underlying_; }
    bool operator != (text_location_range const & other) const
    { return underlying_ != other.underlying_; }

    Range const & underlying() const { return underlying_; }
    std::size_t line() const { return line_; }
    std::size_t column() const { return column_; }
};

namespace operation {

    template <> struct empty <text_location_range_tag, direction::front> {
        template <class Range> bool operator() (
            direction::front, text_location_range <Range> const & r) const
        {
            return ::range::empty (r.underlying());
        }
    };

    template <> struct first <text_location_range_tag, direction::front> {
        template <class Range> auto operator() (
            direction::front, text_location_range <Range> const & r) const
        RETURNS (::range::first (r.underlying()));
    };

    template <> struct drop_one <text_location_range_tag, direction::front> {
        template <class Range, class Increment> Range operator() (
            direction::front, Increment, Range const & r) const
        {
            auto current_character = ::range::first (r.underlying());
            switch (current_character) {
            case '\t':
                return Range (::range::drop (r.underlying()),
                    r.line_, (r.column_ / Range::tab_size + 1)
                        * Range::tab_size, false);
            case '\n':
                if (r.last_carriage_return)
                    // Second character of a CRLF-type newline.
                    return Range (::range::drop (r.underlying()),
                        r.line_, r.column_, false);
                else
                    return Range (::range::drop (r.underlying()),
                        r.line_ + 1, std::size_t(), false);
            case '\r':
                return Range (::range::drop (r.underlying()),
                    r.line_ + 1, std::size_t(), true);
            default:
                return Range (::range::drop (r.underlying()),
                    r.line_, r.column_ + 1, false);
            }
        }
    };

} // namespace operation

} // namespace range

#endif  // PARSE_LL_TEXT_LOCATION_RANGE_HPP_INCLUDED

