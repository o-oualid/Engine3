@echo off
setlocal EnableDelayedExpansion

CD /d %~dp0
if not exist .\Build (
    mkdir .\Build
)
echo Build Started.
cmake -S . -G "Unix Makefiles" -B cmake 1>null

cmake -S . -B build 1>null
cmake --build build
echo Compiling Shaders.



set SRC_DIR=Shaders
set OUT_DIR=Build\Debug\Shaders

REM Create output root directory if it doesn't exist
if not exist "%OUT_DIR%" (
    mkdir "%OUT_DIR%"
)

REM Supported shader extensions
set EXTENSIONS=.vert,.frag 
REM ,".comp",".geom", ".tesc", ".tese"
for %%I in ("%SRC_DIR%") do set "ABS_SRC_DIR=%%~fI"

REM Recursively find shader files
for /R "%SRC_DIR%" %%F in (*.*) do (
    set "FILE=%%~F"
    set "EXT=%%~xF"
    REM Check if the extension matches known shader types
    for %%E in (%EXTENSIONS%) do (
	if /I "%%E"=="!EXT!" (
            REM Get relative path from source directory
            
	    set "REL_PATH=%%F"

	    set "REL_PATH=!REL_PATH:%ABS_SRC_DIR%\=!"
	    set "TRAIL=!EXT:~1%!"
	    set "REL_PATH=!REL_PATH:%%E=!"
	    set "REL_PATH=!REL_PATH!_!TRAIL!.spv"
	    set "OUT_PATH=%OUT_DIR%\!REL_PATH!"
            


            REM Get output directory path
            for %%D in ("!OUT_PATH!") do set "OUT_DIR_PATH=%%~dpD"

            REM Create output directory if it doesn't exist
            if not exist "!OUT_DIR_PATH!" (
                mkdir "!OUT_DIR_PATH!"
            )

            echo Compiling: %%F -> !OUT_PATH!
	    glslc "%%F" -o "!OUT_PATH!"
        )
    )
)

echo Done compiling shaders.

echo Build finished.
endlocal


