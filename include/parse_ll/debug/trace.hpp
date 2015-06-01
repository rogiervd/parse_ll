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

/**
Trace the steps inside parsers while parsing.
*/

#ifndef PARSE_LL_DEBUG_TRACE_HPP_INCLUDED
#define PARSE_LL_DEBUG_TRACE_HPP_INCLUDED

#include <cassert>
#include <memory>
#include <vector>

#include "../core/detail/directive.hpp"

#include "../core/no_skip.hpp"

namespace parse_ll {

namespace parse_policy {

    /**
    This parse function policy that notifies an observer every time something
    is parsed and somthing is returned.
    This is useful for debugging the parser.

    However, lazy parsers are a problem.
    They do no necessarily apply their sub-parsers immediately, but potentially
    later.
    What they do is copy the parse function, including any policies, so they can
    call it later.
    This will yield spurious trace notifications.
    To prevent these, this class needs to detect when it should and when it
    shouldn't generate trace messages.

    It therefore keeps a list of parser depths as shared_ptr's.
    It also keeps a weak_ptr to the greatest depth of the "parent" if it is
    copy-constructed.
    When the "parent" finishes at that depth, it releases the corresponding
    shared_ptr.
    Its copy can detect this by checking whether its parent_depth is expired.
    In that case, it is being called as part of a lazy parse and can be ignored.

    It is not turtles all the way down: wrap_trace (which is in the parse, so
    it must be kept alive as long as parsing is happening) holds the root
    pointer to symbolic depth -1.

    To get a complete parse, succeed() and rest() are always called explicitly.
    This may still yield some spurious not.
    */
    template <class OriginalPolicy, class Observer, class Stop>
        struct trace_policy
    : public OriginalPolicy
    {
        Observer observer;
        Stop stop;

        mutable bool inhibit;
        std::weak_ptr <int> parent_depth;
        mutable std::vector <std::shared_ptr <int>> depths;

        bool inhibited() const { return inhibit || parent_depth.expired(); }

        int current_depth() const {
            assert (!inhibited());
            if (depths.empty())
                return *parent_depth.lock();
            else
                return *depths.back();
        }

    public:
        explicit trace_policy (OriginalPolicy const & original_policy_,
            Observer const & observer, Stop const & stop,
            std::shared_ptr <int> const & root_depth)
        : OriginalPolicy (original_policy_), observer (observer), stop (stop),
            inhibit (false), parent_depth (root_depth) {}

        trace_policy (trace_policy const & other)
        : OriginalPolicy (other), observer (other.observer), stop (other.stop),
          inhibit (other.inhibit) {
            if (other.depths.empty())
                // Simple copy
                parent_depth = other.parent_depth;
            else
                // Dependent copy
                parent_depth = other.depths.back();
        }

        ~trace_policy() {
            /*
            I'm not sure this assertion can never be triggered normally.
            It can be triggered during exception unwinding, which is confusing.
            */
            // assert (parent_depth.expired() || depths.empty());
        }

        template <class Apply, class Policy, class Parser, class Input>
            auto apply_parse (
                Policy const & policy,
                Parser const & parser, Input const & input) const
        // The return type of the original policy.
        -> decltype (std::declval <OriginalPolicy const &>().template
            apply_parse <Apply> (policy, parser, input))
        {
            if (inhibited())
                // Just call the original policy.
                return original_policy().template apply_parse <Apply> (
                    policy, parser, input);
            else {
                int depth = current_depth() + 1;
                depths.push_back (std::make_shared <int> (depth));
                // Notify observer.
                observer.start_parse (depth, parser, input);

                if (stop (parser))
                    inhibit = true;

                // Call the original policy.
                auto outcome = original_policy().template apply_parse <Apply> (
                    policy, parser, input);

                if (inhibit)
                    inhibit = false;

                // Notify observer.
                if (success (outcome)) {
                    // Run sub-parsers through the input once.
                    Input remaining = rest (outcome);
                    // Inhibit any further notifications.
                    depths.pop_back();
                    observer.success (depth, parser, input, remaining, outcome);
                } else {
                    depths.pop_back();
                    observer.no_success (depth, parser, input);
                }
                return std::move (outcome);
            }
        }

        OriginalPolicy const & original_policy() const { return *this; }
    };

    template <class Observer, class Stop> struct wrap_trace {
        Observer observer;
        Stop stop;
        std::shared_ptr <int> root_depth;
    public:
        wrap_trace (Observer const & observer, Stop const & stop)
        : observer (observer), stop (stop),
            root_depth (std::make_shared <int> (-1)) {}

        template <class OriginalPolicy> auto
        operator() (OriginalPolicy const & original_policy) const
        RETURNS (trace_policy <OriginalPolicy, Observer, Stop> (
            original_policy, observer, stop, root_depth));
    };

    struct no_stop {
        template <class Parser>
        bool operator() (Parser const &) const { return false; }
    };

    struct stop_at_no_skip {
        template <class Parser>
        bool operator() (Parser const &) const { return false; }

        template <class SubParser> bool operator() (
            change_policy <SubParser, convert_policy_no_skip> const &) const
        { return true; }
    };

} // namespace parse_policy

/**
Let an observer know about the progress of a parser.
This is useful as instrumentation to a parser for debugging why it does or does
not parse a particular input.
Spurious trace messages from lazy parsers are inhibited.
This does make this policy quite slow.
It is only for debugging.
*/
template <class Observer, class Stop> inline
    change_policy_directive <parse_policy::wrap_trace <Observer, Stop>>
    trace (Observer const & observer, Stop const & stop)
{
    return change_policy_directive <parse_policy::wrap_trace <Observer, Stop>> (
        parse_policy::wrap_trace <Observer, Stop> (observer, stop));
}

template <class Observer> inline auto
    trace (Observer const & observer)
RETURNS (trace (observer, parse_policy::no_stop()));

} // namespace parse_ll

#endif  // PARSE_LL_DEBUG_TRACE_HPP_INCLUDED

