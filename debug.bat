@echo off
CD /d %~dp0
pushd .\Build\Debug
START "Debug" raddbg .\mygame.exe --auto_run -q
popd