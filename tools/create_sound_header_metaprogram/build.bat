@echo off

REM - /S means recursively, /Q means silently
rmdir /S /Q build
mkdir build

cd .\build\
pushd

clang -g ..\create_sound_header_metaprogram.c -o create_sound_header_metaprogram.exe
REM clang -O3 ..\create_sound_header_metaprogram.c -o create_sound_header_metaprogram.exe

popd

REM If there's a NO_COMMIT tag anywhere in the code, we want to know
rg "@NO_COMMIT" --ignore-case -g "!*.bat" -g "!*.sh"
