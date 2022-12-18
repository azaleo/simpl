@echo off

rem This script should not be used directly, please use "build-playground.bat"
rem in the source root instead.

set "compiler_options="
if "%~1" == "debug" (
  set "compiler_options=/Zi"
)

echo playground

mkdir build 2>NUL
mkdir build\playground 2>NUL
pushd build
cl^
 /nologo /W4 /WX /Fo"playground\\" %compiler_options%^
 /I"..\include" "%~dp0src\main.cpp" platform.lib^
 /Fe"playground.exe" /link /NOLOGO
popd

set "compiler_options="
