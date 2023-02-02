@echo off

pushd ..\
if not exist build mkdir build
if not exist build\SDL2.dll copy win32_lib\SDL2\x64\SDL2.dll build
if not exist build\SDL2_image.dll copy win32_lib\SDL2\x64\SDL2_image.dll build
cd build

set linker_flags=-I../code -L../win32_lib/SDL2/x64 -lSDL2 -lSDL2_image
rem -Wl,-subsystem:console
rem -Wl,-subsystem:windows
set compiler_flags=-g -DWINDOWS=1

clang++ %compiler_flags%  ../code/sdl2_main.cpp %linker_flags% -o game.exe

popd
