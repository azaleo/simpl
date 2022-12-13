@echo off

pushd %~dp0
call build_platform.bat
echo:
call modules\playground\build.bat
popd
