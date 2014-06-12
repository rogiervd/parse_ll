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
Test floating-point parser.
*/

#define BOOST_TEST_MODULE float_parser
#include "utility/test/boost_unit_test.hpp"

#include "parse_ll/number/float.hpp"

#include <string>

#include "range/core.hpp"
#include "range/std/container.hpp"

#include <boost/math/special_functions/sign.hpp>

#include "../helper/fuzz_parser.hpp"
#include "parse_ll/core/skip.hpp"
#include "parse_ll/core/literal.hpp"

BOOST_AUTO_TEST_SUITE(test_parse_float)

BOOST_AUTO_TEST_CASE (test_float) {
    using range::empty; using range::first; using range::drop;

    using parse_ll::parse;
    using parse_ll::success;
    using parse_ll::output;
    using parse_ll::rest;
    {
        std::string r ("e+5");
        {
            auto parser = parse_ll::float_exponent;
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            BOOST_CHECK_EQUAL (output (result), 5);

            // Error message contains type:
            //int i = parser;
        }
    }
    {
        std::string r ("52.41");
        {
            auto parser = parse_ll::skip (parse_ll::literal (' ')) [
                parse_ll::mantissa_parser <double>()];
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            BOOST_CHECK_EQUAL (std::get <0>(output (result)), 5241);
        }
    }
    {
        std::string r ("52.41 44");
        {
            // Don't consume the space.
            auto parser = parse_ll::skip (parse_ll::literal (' ')) [
                parse_ll::mantissa_parser <double>()];
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            BOOST_CHECK_EQUAL (std::get <0>(output (result)), 5241);
        }
    }
    {
        std::string r ("52.41e50");
        {
            auto parser = parse_ll::mantissa_parser <double>();
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            BOOST_CHECK_EQUAL (std::get <0>(output (result)), 5241);

            // Error message contains type:
            //int i = parser;
        }
    }

    {
        std::string r ("");
        {
            auto parser = fuzz (parse_ll::float_);
            auto result = parse (parser, r);
            BOOST_CHECK (!success (result));
        }
    }
    {
        std::string r ("a");
        {
            auto parser = parse_ll::float_;
            auto result = parse (parser, r);
            BOOST_CHECK (!success (result));
        }
    }
    {
        std::string r ("0");
        {
            auto parser = fuzz (parse_ll::float_);
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            BOOST_CHECK_EQUAL (output (result), 0);
            BOOST_CHECK (empty (rest (result)));
        }
    }
    {
        std::string r ("-0");
        {
            auto parser = fuzz (parse_ll::float_as <float>());
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            BOOST_CHECK_EQUAL (output (result), -0);
            BOOST_CHECK (boost::math::signbit (output (result)));
            BOOST_CHECK (empty (rest (result)));
        }
    }
    {
        std::string r ("+6.25 99");
        {
            auto parser = parse_ll::skip (parse_ll::literal (' ')) [
                parse_ll::float_];
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            BOOST_CHECK_EQUAL (output (result), 6.25);
            BOOST_CHECK_EQUAL (first (rest (result)), ' ');
        }
    }
    {
        std::string r ("+6.25e50");
        {
            auto parser = fuzz (parse_ll::float_);
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            BOOST_CHECK_EQUAL (output (result), 6.25e50);
            BOOST_CHECK (empty (rest (result)));
        }
        {
            auto parser = fuzz (parse_ll::float_);
            auto o = output (parse (parser, r));
            BOOST_CHECK_EQUAL (o, 6.25e50);
        }
    }
    {
        std::string r ("-6371");
        {
            auto parser = fuzz (parse_ll::float_);
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            BOOST_CHECK_EQUAL (output (result), -6371);
            BOOST_CHECK (empty (rest (result)));
        }
    }
    {
        std::string r ("-.3654");
        {
            auto parser = parse_ll::float_as <float>();
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            BOOST_CHECK_EQUAL (output (result), -.3654f);
        }
    }
    {
        std::string r ("-0.3654");
        {
            auto parser = parse_ll::float_as <float>();
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            BOOST_CHECK_EQUAL (output (result), -.3654f);
        }
    }
    {
        std::string r ("1984756.2345952301358780564e50");
        {
            auto parser = fuzz (parse_ll::float_as <double>());
            auto result = parse (parser, r);
            BOOST_CHECK (success (result));
            BOOST_CHECK_EQUAL (output (result), 1984756.2345952301358780564e50);
        }
    }

    // \todo Test overflow errors?
    /*
    // Overflow when the exponent gets added
    {
        std::string r ("1984756.2345952301358780564e50");
        {
            auto parser = parse_ll::float_as <float>();
            auto result = output (parse (parser, r));
        }
    }
    // Overflow while constructing the float should not happen.
    {
        std::string r ("1984756.23459523013587805644531854651321354651321");
        {
            auto parser = parse_ll::float_as <float>();
            auto result = output (parse (parser, r));
            BOOST_CHECK (success (result));
            BOOST_CHECK_EQUAL (output (result),
                1984756.23459523013587805644531854651321354651321f);
        }
    }*/

}

BOOST_AUTO_TEST_SUITE_END()
