#!/bin/bash

# Run this from the root directory of the "parse_ll" repository.

# This script will generate a directory ../parse_ll-test and not remove it!
# So watch out where you run it.

# Run the tests by using this repository as submodule of the "parse_ll-test"
# repository.

# This is necessary because this repository cannot be tested by itself.


# Travis CI looks for this line.
set -ev

set -o nounset
set -o errexit

(
    # Check out the "master" branch of "parse_ll-test" from GitHub, in the parent
    # directory.

    cd ../
    git clone git://github.com/rogiervd/parse_ll-test.git
    cd parse_ll-test
    git checkout master
    git submodule init
    git submodule update
    # Then replace the "parse_ll" submodule with the one in ../parse_ll.
    rm -r parse_ll
    ln -s ../parse_ll

    # Test it
    bjam test "$@"

)
