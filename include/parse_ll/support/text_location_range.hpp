/*
Copyright 2012, 2015 Rogier van Dalen.

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
Define a range that remembers its position in a text file.
*/

#ifndef PARSE_LL_TEXT_LOCATION_RANGE_HPP_INCLUDED
#define PARSE_LL_TEXT_LOCATION_RANGE_HPP_INCLUDED

#include "utility/returns.hpp"

#include "range/core.hpp"

namespace range {

template <class Range> class text_location_range;

struct text_location_range_tag {};

template <class Range> struct tag_of_qualified <text_location_range <Range>>
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

private:
    friend class range::helper::member_access;

    bool empty (direction::front) const
    { return ::range::empty (underlying()); }

    auto first (direction::front) const
    RETURNS (::range::first (underlying_));

    text_location_range drop_one (direction::front) const {
        auto current_character = ::range::first (underlying_);
        switch (current_character) {
        case '\t':
            return text_location_range (::range::drop (underlying_),
                line_, (column_ / tab_size + 1) * tab_size, false);
        case '\n':
            if (last_carriage_return)
                // Second character of a CRLF-type newline.
                return text_location_range (::range::drop (underlying_),
                    line_, column_, false);
            else
                return text_location_range (::range::drop (underlying_),
                    line_ + 1, std::size_t(), false);
        case '\r':
            return text_location_range (::range::drop (underlying_),
                line_ + 1, std::size_t(), true);
        default:
            return text_location_range (::range::drop (underlying_),
                line_, column_ + 1, false);
        }
    }
};

} // namespace range

#endif  // PARSE_LL_TEXT_LOCATION_RANGE_HPP_INCLUDED
