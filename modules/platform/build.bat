@echo off

rem This script should not be used directly, please use "build_platform.bat"
rem in the source root instead.

set "SOURCES="
call :add_source_to SOURCES source\lib.cpp

set "TEST_SOURCES="
call :add_source_to TEST_SOURCES tests\main.cpp

rem For now compiler options are used for both lib and tests.
set "COMPILER_OPTIONS="
if "%~1" == "debug" (
  call :add_to COMPILER_OPTIONS /Zi
)

echo:
echo platform

mkdir build 2>NUL
mkdir build\platform 2>NUL
pushd build
cl^
 /nologo /W4 /WX /Fo"platform\\" %COMPILER_OPTIONS%^
 /I"..\include" /c %SOURCES%^
 && lib /NOLOGO /WX "platform\*.obj"^
 /OUT:platform.lib
popd

echo:
echo platform tests

mkdir build\platform\tests 2>NUL
pushd build\platform
cl^
 /nologo /W4 /WX /Fo"tests\\" %COMPILER_OPTIONS%^
 /I"..\..\include" %TEST_SOURCES% ..\platform.lib^
 /Fe"tests.exe" /link /NOLOGO
popd

set "SOURCES="
set "TEST_SOURCES="
set "COMPILER_OPTIONS="
goto :eof

:add_to
if not defined %1 (
  set "%1=%2"
) else (
  set "%1=%%1% %2"
)
exit /b 0

:add_source_to
call :add_to %1 "%~dp0%~2"
exit /b 0
