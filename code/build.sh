#!/usr/bin/bash

pushd ..

mkdir -p build
cd build

linker_flags="-I../code -L../linux_lib/ -lSDL2 -lSDL2_image -lSDL2_ttf"
compiler_flags="-g -std=c++20 -Werror -Wall -Wno-c99-designator"

set -xe

clang++ $compiler_flags  ../code/main.cpp $linker_flags -o game

popd
