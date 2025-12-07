#!/bin/bash
if git rev-parse --show-toplevel > /dev/null 2>&1; then
    if [ "$(git rev-parse --show-toplevel)" = "$(pwd)" ]; then
        echo "You are in the root of a Git project."
    else
        echo "You are NOT in the root of a Git project."
    fi
else
    echo "This is not a Git repository."
fi
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ..
make
./MollenLang ../Tests/helloWord.mol