@echo off

rem This script should not be used directly, please use "build-platform.bat"
rem in the source root instead.

set "sources="
set "test_sources="
for /f %%i in (%~dp0sources.txt) do call :add_source_to sources %%i
for /f %%i in (%~dp0test-sources.txt) do call :add_source_to test_sources %%i

rem For now compiler options are used for both lib and tests.
set "compiler_options="
if "%~1" == "debug" (
  call :add_to compiler_options /Zi
)

echo platform

mkdir build 2>NUL
mkdir build\platform 2>NUL
pushd build
cl^
 /nologo /W4 /WX /Fo"platform\\" %compiler_options%^
 /I"..\include" /c %sources%^
 && lib /NOLOGO /WX "platform\*.obj"^
 /OUT:platform.lib
popd

echo:
echo platform\tests

mkdir build\platform\tests 2>NUL
pushd build\platform
cl^
 /nologo /W4 /WX /Fo"tests\\" %compiler_options%^
 /I"..\..\include" %test_sources% ..\platform.lib^
 /Fe"tests.exe" /link /NOLOGO
popd

set "sources="
set "test_sources="
set "compiler_options="
goto :eof

rem Slashes in the source path are normalized into backslashes.
:add_source_to
set "source=%~dp0%~2"
call :add_to %1 "%source:/=\%"
set "source="
exit /b 0

:add_to
set "var_name=%1"
if not defined %var_name% (
  set "%var_name%=%2"
) else (
  call set "%var_name%=%%%var_name%%% %2"
)
set "var_name="
exit /b 0
