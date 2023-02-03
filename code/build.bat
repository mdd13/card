@echo off

pushd ..\
if not exist build mkdir build
if not exist build\SDL2.dll copy win32_lib\SDL2\x64\SDL2.dll build
if not exist build\SDL2_image.dll copy win32_lib\SDL2\x64\SDL2_image.dll build
if not exist build\SDL2_ttf.dll copy win32_lib\SDL2\x64\SDL2_ttf.dll build
cd build

set linker_flags=-I../code -L../win32_lib/SDL2/x64 -lSDL2 -lSDL2_image -lSDL2_ttf
rem -Wl,-subsystem:console
rem -Wl,-subsystem:windows
set compiler_flags=-g -DWINDOWS=1 -std=c++20 -Werror -Wall -Wno-c99-designator

clang++ %compiler_flags%  ../code/main.cpp %linker_flags% -o game.exe

popd
