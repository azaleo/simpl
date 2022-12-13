@echo off

rem This script should not be used directly, please use "build_playground.bat"
rem in the source root instead.

echo:
echo playground

mkdir build 2>NUL
mkdir build\playground 2>NUL
pushd build
cl^
 /nologo /W4 /WX /Fo"playground\\"^
 /I"..\include" "%~dp0src\main.cpp" platform.lib^
 /Fe"playground.exe" /link /NOLOGO
popd
