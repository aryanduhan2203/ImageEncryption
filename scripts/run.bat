@echo off
REM ===================================================================
REM  run.bat - Execute the ImageEncryption application
REM ===================================================================
REM
REM Usage:
REM   .\scripts\run.bat help
REM   .\scripts\run.bat encrypt image.jpg -p password
REM   .\scripts\run.bat decrypt image.enc -p password -o output.jpg
REM
REM ===================================================================

setlocal enabledelayedexpansion

REM Get the project root directory
set PROJECT_ROOT=%~dp0..
set EXECUTABLE=%PROJECT_ROOT%\build\Release\imgenc.exe

REM Check if executable exists
if not exist "%EXECUTABLE%" (
    echo Error: Executable not found at %EXECUTABLE%
    echo Please run build_fast.bat first.
    exit /b 1
)

REM Run the application with all provided arguments
echo Running ImageEncryption...
echo.
"%EXECUTABLE%" %*

endlocal
