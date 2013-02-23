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
Wrap Boost.Spirit's istream_iterator in a range.
*/

#ifndef PARSE_LL_SUPPORT_FILE_RANGE_HPP
#define PARSE_LL_SUPPORT_FILE_RANGE_HPP

#include <memory>
#include <fstream>
#include <boost/spirit/include/support_istream_iterator.hpp>

#include "range/core.hpp"
#include "range/std/container.hpp"

namespace range {

class file_range;
struct file_range_tag;

template <> struct tag_of_bare <file_range>
{ typedef file_range_tag type; };

/**
\todo
Produce a proper version of this; the current one may be quite wasteful,
keeping the file around in a few different senses.
This means that copy and move assignment are probably not as safe as they
should be.
*/
class file_range {
public:
    std::shared_ptr <std::ifstream> file;
    typedef boost::spirit::istream_iterator iterator;
    iterator begin;
    iterator end;

    file_range (std::shared_ptr <std::ifstream> const & file, iterator begin)
    : file (file), begin (begin), end() {}
public:
    file_range (std::string const & file_name) {
        auto new_file = std::make_shared <std::ifstream>();
        new_file->exceptions (std::ifstream::failbit | std::ifstream::badbit);
        new_file->open (file_name.c_str());
        new_file->unsetf(std::ios::skipws);

        // No exceptions raised, so remember the file.
        file = new_file;
        begin = iterator (*file);
        // end is set to the past-the-end iterator already.
    }

    // This should have a move constructor and assignment; that's a feature!
    file_range (file_range const & other) = default;
    file_range (file_range && other)
    : file (std::move (other.file)), begin (std::move (other.begin)), end() {}

    /**
    Basic exception guarantee.
    (The file may end up referring to a different file than the iterator...!)
    */
    file_range & operator = (file_range const & other) = default;

    /**
    Basic exception guarantee.
    (The file may end up referring to a different file than the iterator...!)
    */
    file_range & operator = (file_range && other) {
        this->file = std::move (other.file);
        this->begin = std::move (other.begin);
        // this->end is fine.
        return *this;
    }

    bool operator == (file_range const & other) const
    { return begin == other.begin; }
    bool operator != (file_range const & other) const
    { return !(this->begin == other.begin); }
};

namespace operation {
    template<> struct empty <file_range_tag, direction::front> {
        bool operator() (direction::front, file_range const & r) const
        { return r.begin == r.end; }
    };

    template<> struct first <file_range_tag, direction::front> {
        char operator() (direction::front, file_range const & r) const
        { return *r.begin; }
    };

    template<> struct drop_one <file_range_tag, direction::front> {
        template <class Increment>
        file_range operator() (
            direction::front, Increment, file_range const & r) const
        {
            file_range::iterator next = r.begin;
            ++ next;
            return file_range (r.file, next);
        }
    };
} // namespace operation

} // namespace range

#endif  // PARSE_LL_SUPPORT_FILE_RANGE_HPP

