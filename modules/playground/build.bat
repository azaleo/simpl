@echo off

rem This script should not be used directly, please use "build_playground.bat"
rem in the source root instead.

mkdir build 2> NUL
mkdir build\playground 2> NUL
pushd build
cl^
 /W4 /WX /Fo"playground\\"^
 /I"..\include" "%~dp0source\main.cpp" simpl_platform.lib^
 /Fe"simpl_playground.exe"
popd
