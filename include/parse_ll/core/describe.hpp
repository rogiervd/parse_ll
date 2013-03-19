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
\file
Provide a basic "describe" function for any parser
*/

#ifndef PARSE_LL_BASE_DESCRIBE_HPP_INCLUDED
#define PARSE_LL_BASE_DESCRIBE_HPP_INCLUDED

#include <typeinfo>
#include "core.hpp"

namespace parse_ll { namespace operation {

    /**
    Generic implementation that uses typeid.
    */
    template <class ParserTag> struct describe <ParserTag> {
        template <class Parser> const char * operator() (Parser const &) const
        { return typeid (Parser).name(); }
    };

}} // namespace parse_ll::operation

#endif  // PARSE_LL_BASE_DESCRIBE_HPP_INCLUDED
