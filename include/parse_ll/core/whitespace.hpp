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
