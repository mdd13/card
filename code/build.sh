#!/usr/bin/bash

pushd ..

mkdir -p build
cd build

linker_flags="-I../code -L../linux_lib/ -lSDL2"
compiler_flags="-g -DWINDOWS=1"

set -xe

clang++ $compiler_flags  ../code/sdl2_main.cpp $linker_flags -o game

popd


