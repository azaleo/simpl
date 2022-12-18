@echo off

pushd %~dp0
call build-platform.bat
echo:
call modules\playground\build.bat
popd
