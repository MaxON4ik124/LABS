@echo off
setlocal

cd /d "%~dp0"

set "MINGW=C:\MinGW\w64devkit"
set "PATH=%MINGW%\bin;%PATH%"

rem Build a Windows library from the same sources as the included headers.
rem Do not link the incompatible ELF archive in the global MinGW lib folder.
"%MINGW%\bin\cmake.exe" ^
    -S "vendor/zipper" ^
    -B "build/zipper-mingw" ^
    -G "MinGW Makefiles" ^
    -DCMAKE_C_COMPILER:FILEPATH="C:/MinGW/w64devkit/bin/gcc.exe" ^
    -DCMAKE_CXX_COMPILER:FILEPATH="C:/MinGW/w64devkit/bin/g++.exe" ^
    -DCMAKE_MAKE_PROGRAM:FILEPATH="C:/MinGW/w64devkit/bin/make.exe" ^
    -DZLIB_LIBRARY:FILEPATH="C:/MinGW/w64devkit/lib/libz.a" ^
    -DZLIB_INCLUDE_DIR:PATH="C:/MinGW/w64devkit/include" ^
    -DBUILD_SHARED_VERSION=OFF ^
    -DBUILD_STATIC_VERSION=ON ^
    -DBUILD_TEST=OFF ^
    -DCMAKE_BUILD_TYPE=Release

if errorlevel 1 goto failed

"%MINGW%\bin\cmake.exe" --build "build/zipper-mingw" ^
    --target Zipper-static --parallel 2

if errorlevel 1 goto failed

"%MINGW%\bin\g++.exe" ^
    -std=c++17 ^
    -I"vendor/zipper" ^
    -I"%MINGW%\include" ^
    "zipper.cpp" ^
    "build/zipper-mingw/libZipper-static.a" ^
    "%MINGW%\lib\libz.a" ^
    -ladvapi32 ^
    -lshlwapi ^
    -o "zipper.exe"

if errorlevel 1 goto failed

echo Compilation successful
if /I not "%~1"=="--no-pause" pause
exit /b 0

:failed
echo Compilation failed
if /I not "%~1"=="--no-pause" pause
exit /b 1
