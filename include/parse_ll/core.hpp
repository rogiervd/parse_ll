/*
Copyright 2012, 2013 Rogier van Dalen.

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
Include everything from "core".
*/

#ifndef PARSE_LL_BASE_HPP_INCLUDED
#define PARSE_LL_BASE_HPP_INCLUDED

#include "core/fwd.hpp"
#include "core/outcome.hpp"
#include "core/core.hpp"

// Terminal parsers
#include "core/literal.hpp"
#include "core/char.hpp"
#include "core/nothing.hpp"
#include "core/end.hpp"

// Skip
#include "core/no_skip.hpp"
#include "core/skip.hpp"

// Structured parsers
#include "core/alternative.hpp"
#include "core/difference.hpp"
#include "core/optional.hpp"
#include "core/repeat.hpp"
#include "core/sequence.hpp"
#include "core/transform.hpp"

// Miscellaneous
#include "core/named.hpp"
#include "core/error.hpp"
#include "core/rule.hpp"
#include "core/whitespace.hpp"

#endif  // PARSE_LL_BASE_HPP_INCLUDED

