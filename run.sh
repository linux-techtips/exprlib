set -xe

cmake -S . -B build
bear -- cmake --build build
./build/tests/entry