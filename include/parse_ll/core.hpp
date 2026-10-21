// SPDX-FileCopyrightText: Copyright 2012, 2013 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

/** \file
Include everything from "core".
*/

#ifndef PARSE_LL_BASE_HPP_INCLUDED
#define PARSE_LL_BASE_HPP_INCLUDED

#include "core/core.hpp"
#include "core/fwd.hpp"
#include "core/outcome.hpp"

// Terminal parsers
#include "core/char.hpp"
#include "core/end.hpp"
#include "core/literal.hpp"
#include "core/nothing.hpp"

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
#include "core/error.hpp"
#include "core/named.hpp"
#include "core/rule.hpp"
#include "core/whitespace.hpp"

#endif  // PARSE_LL_BASE_HPP_INCLUDED
