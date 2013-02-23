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
Define whitespace parsers.
*/

#ifndef PARSE_LL_WHITESPACE_HPP_INCLUDED
#define PARSE_LL_WHITESPACE_HPP_INCLUDED

#include "fwd.hpp"
#include "core.hpp"
#include "literal.hpp"
#include "alternative.hpp"
#include "sequence.hpp"
#include "repeat.hpp"
#include "optional.hpp"

namespace parse_ll {

// horizontal_space
static const auto space = literal (' ');
static const auto tab = literal ('\t');
static const auto one_horizontal_space = (space | tab);
static const auto horizontal_space = *one_horizontal_space;

static const auto line_feed = literal ('\n');
static const auto carriage_return = literal ('\r');
static const auto newline = line_feed | (carriage_return >> -line_feed);
static const auto one_vertical_space = +(line_feed | carriage_return);
static const auto vertical_space = //*one_vertical_space;
    *(line_feed | carriage_return);

static const auto one_whitespace = one_horizontal_space | one_vertical_space;
static const auto whitespace = *(space | tab | line_feed | carriage_return);

} // namespace parse_ll

#endif  // PARSE_LL_WHITESPACE_HPP_INCLUDED

