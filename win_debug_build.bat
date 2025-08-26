@echo off
REM - @echo off basically says "Don't print the script to the command line, just the output".

REM The default encoding for command prompt is Windows-1252.
REM This changes it to utf-8.
chcp 65001

REM - Setup Visual Studio to be able to build from Command Prompt (See the very first video of Handmade Hero for more info, around 20 minute mark)
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64

REM - /S means recursively, /Q means silently
rmdir /S /Q build\debug\windows
mkdir build\debug\windows
pushd build\debug\windows

REM - COMPILATION:
REM
REM - /nologo removes annoying banner
REM - /Zi produces debug info
REM - /utf-8 - set source and execution charset to UTF-8
REM - /validate-charset - validate UTF-8 files
cl /nologo /Zi /utf-8 /validate-charset gdi32.lib msvcrt.lib raylib.lib winmm.lib user32.lib shell32.lib ..\..\..\src\gasoline_metronome.c -I..\..\..\src\lib\raylib\raylib-5.5_win64_msvc16\include /link /libpath:..\..\..\src\lib\raylib\raylib-5.5_win64_msvc16\lib /NODEFAULTLIB:libcmt

popd

echo\
REM If there's a NO_COMMIT tag anywhere in the code, we want to know
rg "@NO_COMMIT" --ignore-case -g "!win_debug_build.bat"
echo\

REM ...and back to Windows-1252
chcp 1252
