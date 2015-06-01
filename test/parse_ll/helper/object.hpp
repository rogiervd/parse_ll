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

