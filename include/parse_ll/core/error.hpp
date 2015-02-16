/*
Copyright 2012, 2014, 2015 Rogier van Dalen.

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
#include <string>
#include <boost/exception/all.hpp>

namespace parse_ll {

/**
Exception class that represents a parse error.
*/
struct error
: public virtual boost::exception, public virtual std::exception {};

typedef boost::error_info <struct parse_error_tag, std::string>
    error_description;

template <class Input> struct error_position_tag;

template <class Input> struct error_position
{ typedef boost::error_info <error_position_tag <Input>, Input> type; };

/**
Return a marker of the position where a parser error occurred.
\todo For some types of errors, the actual range should possibly not be kept:
it possibly keeps too much information, yet does not naturally allow one to
show the text of the whole line where the error occurred.
\todo Provide a facility that outputs something helpful to a stream.
*/
template <class Input> inline
    typename error_position <Input>::type error_at (Input position)
{ return typename error_position <Input>::type (std::move (position)); }

} // namespace parse_ll

#endif  // PARSE_LL_BASE_ERROR_HPP_INCLUDED
