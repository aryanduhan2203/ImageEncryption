@echo off
REM ===================================================================
REM  clean.bat - Clean build artifacts
REM ===================================================================
REM
REM Usage:
REM   .\scripts\clean.bat
REM   .\scripts\clean.bat all
REM
REM ===================================================================

setlocal enabledelayedexpansion

REM Get the project root directory
set PROJECT_ROOT=%~dp0..
cd /d "%PROJECT_ROOT%"

echo.
echo ===================================================================
echo Cleaning build artifacts...
echo ===================================================================
echo.

REM Remove build directory
if exist build (
    echo Removing build directory...
    rmdir /s /q build
    if !errorlevel! equ 0 (
        echo.
        echo ✓ Build directory removed
    ) else (
        echo Error: Failed to remove build directory
        exit /b 1
    )
)

REM Option to clean temporary files
if "%1"=="all" (
    echo.
    echo Removing temporary files...
    
    if exist .vs (
        rmdir /s /q .vs
        echo ✓ Visual Studio cache removed
    )
    
    for /r . %%f in (*.o *.obj *.lib *.a) do (
        del /f /q "%%f" >nul 2>&1
    )
    echo ✓ Object files removed
)

echo.
echo ===================================================================
echo Clean completed successfully!
echo ===================================================================
echo.

endlocal
exit /b 0
