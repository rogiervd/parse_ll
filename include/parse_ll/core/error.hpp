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
Define an exception class for parses.
*/

#ifndef PARSE_LL_BASE_ERROR_HPP_INCLUDED
#define PARSE_LL_BASE_ERROR_HPP_INCLUDED

#include <stdexcept>

namespace parse_ll {

/**
Exception class that represents a parse error.
*/
class error : public std::runtime_error
{
public:
    error () : std::runtime_error ("Parse error") {}
};

/**
Exception class that represents a parse error that keeps the position in which
the error occurred, so that it can be presented to the user.
*/
template <class Input> class error_at : public error
{
    Input position_;
public:
    error_at (Input const & position_)
    : position_ (position_) {}

    // \todo Add test (text_location_range <file_range> seems to do it) that
    // triggers the error message on gcc that this solves.
    virtual ~error_at() noexcept (true) {}

    Input const & position() const { return position_; }
};

} // namespace parse_ll

#endif  // PARSE_LL_BASE_ERROR_HPP_INCLUDED

