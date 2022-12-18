@echo off

pushd %~dp0
call build-platform.bat %1
echo:
call modules\playground\build.bat %1
popd
