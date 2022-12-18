@echo off

pushd %~dp0
call modules\platform\build.bat %1
popd
