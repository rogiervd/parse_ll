// SPDX-FileCopyrightText: Copyright 2012 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

/** \file
Define whitespace parsers.
*/

#ifndef PARSE_LL_WHITESPACE_HPP_INCLUDED
#define PARSE_LL_WHITESPACE_HPP_INCLUDED

#include "alternative.hpp"
#include "core.hpp"
#include "fwd.hpp"
#include "literal.hpp"
#include "optional.hpp"
#include "repeat.hpp"
#include "sequence.hpp"

namespace parse_ll {

// horizontal_space
static auto const space = literal(' ');
static auto const tab = literal('\t');
static auto const one_horizontal_space = (space | tab);
static auto const horizontal_space = *one_horizontal_space;

static auto const line_feed = literal('\n');
static auto const carriage_return = literal('\r');
static auto const newline = line_feed | (carriage_return >> -line_feed);
static auto const vertical_space = *(line_feed | carriage_return);  // *newline

static auto const one_whitespace = one_horizontal_space | newline;
static auto const whitespace = *(space | tab | line_feed | carriage_return);

}  // namespace parse_ll

#endif  // PARSE_LL_WHITESPACE_HPP_INCLUDED
