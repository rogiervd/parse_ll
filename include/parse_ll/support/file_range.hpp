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

template <> struct tag_of_qualified <file_range>
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
    template <class Range>
        struct empty <file_range_tag, direction::front, Range>
    {
        bool operator() (direction::front, file_range const & r) const
        { return r.begin == r.end; }
    };

    template <class Range>
        struct first <file_range_tag, direction::front, Range>
    {
        char operator() (direction::front, file_range const & r) const
        { return *r.begin; }
    };

    template <class Range>
        struct drop_one <file_range_tag, direction::front, Range>
    {
        template <class Increment> file_range operator() (
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

