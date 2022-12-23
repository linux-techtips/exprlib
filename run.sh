#!/bin/bash

if [ ! -d build ]; then
    mkdir build
fi

set -xe

cmake -DCMAKE_BUILD_TYPE=ASAN -DCMAKE_BUILD_TYPE=Debug -S . -GNinja -B build
bear -- cmake --build build
time ./build/tests/entry