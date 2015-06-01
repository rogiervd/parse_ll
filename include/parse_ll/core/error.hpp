/*
Copyright 2012, 2014, 2015 Rogier van Dalen.

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
