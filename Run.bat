@echo off
CD /d %~dp0
pushd .\Build\Debug
START "Run" .\mygame.exe
popd
