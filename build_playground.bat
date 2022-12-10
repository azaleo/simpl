@echo off

pushd %~dp0
call build_platform.bat
call modules\playground\build.bat
popd
