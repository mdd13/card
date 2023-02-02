#!/usr/bin/bash

pushd ..

mkdir -p build
cd build

linker_flags="-I../code -L../linux_lib/ -lSDL2 -lSDL2_image"
compiler_flags="-O3"

set -xe

clang++ $compiler_flags  ../code/sdl2_main.cpp $linker_flags -o game

popd


