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
