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

#ifndef PARSE_TEST_HELPER_OBJECT_HPP_INCLUDED
#define PARSE_TEST_HELPER_OBJECT_HPP_INCLUDED

#include <cassert>
#include <ostream>

class object {
    static constexpr int initialised = 0x4DF5;
    static constexpr int uninitialised = 0;
    int pattern;
    int id_;
public:
    object (int id_) : pattern (initialised), id_ (id_) {
//        std::cout << "Initialising test object" << std::endl;
    }

    object() : pattern (initialised), id_ (-1) {}

    object (object const & other) : pattern (initialised), id_ (other.id_) {
//        std::cout << "Copying test object" << std::endl;
        assert (other.pattern == initialised);
    }

    ~object() {
//        std::cout << "Destructing test object" << std::endl;
        pattern = uninitialised;
    }

    bool operator == (object const &other) const {
        assert (pattern == initialised);
        assert (other.pattern == initialised);
        return id_ == other.id_;
    }

    int id() const {
        assert (pattern == initialised);
        return id_;
    }
};

inline std::ostream & operator << (std::ostream & s, object const & o) {
    return s << o.id();
}


/**
A very limited version of Boost.Phoenix's value that does make for simpler
error messages.
*/
template <typename Result> class always_return {
    Result result;
public:
    always_return (Result const & result) : result (result) {}

    template <typename ... Arguments>
    Result operator() (Arguments const & ...) const { return result; }
};
template <> class always_return <void> {
public:
    always_return () {}

    template <typename ... Arguments>
    void operator() (Arguments const & ...) const {}
};

template <typename Result>
    inline always_return <Result> always (Result const & result) {
    return always_return <Result> (result);
}

always_return <void> always() { return always_return <void>(); }


#endif  // PARSE_TEST_HELPER_OBJECT_HPP_INCLUDED

