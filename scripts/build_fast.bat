@echo off
REM ===================================================================
REM  build_fast.bat - Quick build of ImageEncryption (no tests)
REM ===================================================================
REM
REM Usage:
REM   .\scripts\build_fast.bat
REM   .\scripts\build_fast.bat rebuild
REM   .\scripts\build_fast.bat clean
REM
REM ===================================================================

setlocal enabledelayedexpansion

REM Get the project root directory
set PROJECT_ROOT=%~dp0..
cd /d "%PROJECT_ROOT%"

echo.
echo ===================================================================
echo ImageEncryption - Fast Build (No Tests)
echo ===================================================================
echo.
echo Project Root: %PROJECT_ROOT%
echo.

REM Check if CMake is installed
where cmake >nul 2>&1
if %errorlevel% neq 0 (
    echo Error: CMake not found. Please install CMake.
    exit /b 1
)

REM Determine build flag
set BUILD_FLAG=--build
if "%1"=="rebuild" (
    set BUILD_FLAG=--build
    set CMAKE_CLEAN=--clean-first
    echo Building with --clean-first
) else if "%1"=="clean" (
    echo Cleaning build directory...
    if exist build (
        rmdir /s /q build
    )
    echo Build directory cleaned.
    exit /b 0
)

REM Create build directory if it doesn't exist
if not exist build (
    echo Creating build directory...
    mkdir build
)

REM Configure the project
echo.
echo [1/3] Running CMake configuration...
cd build
cmake -G "Visual Studio 16 2019" -A x64 ..
if !errorlevel! neq 0 (
    echo Error: CMake configuration failed
    exit /b 1
)
cd ..

REM Build the project
echo.
echo [2/3] Building project...
cmake --build build --config Release
if !errorlevel! neq 0 (
    echo Error: Build failed
    exit /b 1
)

REM Report success
echo.
echo [3/3] Build completed successfully!
echo.
echo ===================================================================
echo Executable location: build\Release\imgenc.exe
echo ===================================================================
echo.

endlocal
exit /b 0
