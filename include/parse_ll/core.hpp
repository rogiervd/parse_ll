/*
Copyright 2012, 2013 Rogier van Dalen.

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
