@echo off

REM - /S means recursively, /Q means silently
rmdir /S /Q build
mkdir build

clang -g create_sound_header_metaprogram.c -o build\create_sound_header_metaprogram.exe
