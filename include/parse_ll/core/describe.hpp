// SPDX-FileCopyrightText: Copyright 2012 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

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
    template <class ParserTag> struct describe<ParserTag>
    {
        template <class Parser> char const * operator()(Parser const &) const
        {
            return typeid(Parser).name();
        }
    };

}}  // namespace parse_ll::operation

#endif  // PARSE_LL_BASE_DESCRIBE_HPP_INCLUDED
