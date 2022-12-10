@echo off

rem This script should not be used directly, please use "build_platform.bat"
rem in the source root instead.

set "SOURCES="
call :add_source source\lib.cpp

set "EXTRA_OPTIONS="
if "%~1" == "debug" (
  call :add_extra_option /Zi
)

mkdir build 2> NUL
mkdir build\platform 2> NUL
pushd build
cl^
 /W4 /WX /Fo"platform\\" %EXTRA_OPTIONS%^
 /I"..\include" /c %SOURCES%^
 && lib /WX "platform\*.obj" /OUT:simpl_platform.lib
popd

set "SOURCES="
set "EXTRA_OPTIONS="
goto :eof

:add_source
set "ABS_PATH="%~dp0%~1""
if defined SOURCES (
  set "SOURCES=%SOURCES% %ABS_PATH%"
) else (
  set "SOURCES=%ABS_PATH%"
)

set "ABS_PATH="
exit /b 0

:add_extra_option
set "OPTION=%~1"
if defined EXTRA_OPTIONS (
  set "EXTRA_OPTIONS=%EXTRA_OPTIONS% %OPTION%"
) else (
  set "EXTRA_OPTIONS=%OPTION%"
)

set "OPTION="
exit /b 0
