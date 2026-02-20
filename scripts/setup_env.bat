@echo off
REM ===================================================================
REM  setup_env.bat - Setup development environment on Windows
REM ===================================================================
REM
REM Checks for and displays information about:
REM - CMake
REM - Visual Studio Build Tools
REM - Git
REM - OpenSSL
REM - OpenCV
REM
REM Usage:
REM   .\scripts\setup_env.bat
REM
REM ===================================================================

echo.
echo ===================================================================
echo ImageEncryption - Environment Setup Check
echo ===================================================================
echo.

REM Check CMake
echo Checking CMake...
where cmake >nul 2>&1
if %errorlevel% equ 0 (
    echo ✓ CMake is installed
    cmake --version | findstr /R "cmake" | (set /p CMAKE_VER= && echo   Version: !CMAKE_VER!)
) else (
    echo ✗ CMake not found - Please install from https://cmake.org/download/
)
echo.

REM Check Visual Studio Build Tools or Studio Community
echo Checking Visual Studio...
where devenv >nul 2>&1
if %errorlevel% equ 0 (
    echo ✓ Visual Studio found
) else (
    where msbuild >nul 2>&1
    if %errorlevel% equ 0 (
        echo ✓ MSBuild found (Visual Studio Build Tools)
    ) else (
        echo ✗ Visual Studio not found - Please install from:
        echo   https://visualstudio.microsoft.com/downloads/
    )
)
echo.

REM Check Git
echo Checking Git...
where git >nul 2>&1
if %errorlevel% equ 0 (
    echo ✓ Git is installed
    git --version
) else (
    echo ✗ Git not found - Please install from https://git-scm.com/
)
echo.

REM Check OpenSSL
echo Checking OpenSSL...
where openssl >nul 2>&1
if %errorlevel% equ 0 (
    echo ✓ OpenSSL is installed
    openssl version
) else (
    echo ✗ OpenSSL command not in PATH
    echo   Note: It may still be installed but not in PATH
)
echo.

REM Check for OpenCV (harder to verify)
echo Checking OpenCV...
if exist "C:\opencv" (
    echo ✓ OpenCV directory found at C:\opencv
) else if exist "%PROGRAMFILES%\OpenCV" (
    echo ✓ OpenCV directory found at %PROGRAMFILES%\OpenCV
) else (
    echo ℹ OpenCV directory not found in standard locations
    echo   Make sure OpenCV is installed and CMAKE_PREFIX_PATH is set
)
echo.

REM Check for Python (useful for scripts)
echo Checking Python...
where python >nul 2>&1
if %errorlevel% equ 0 (
    echo ✓ Python is installed
    python --version
) else (
    echo ℹ Python not found in PATH (optional)
)
echo.

echo ===================================================================
echo Summary:
echo ===================================================================
echo.
echo To build the project:
echo   .\scripts\build_fast.bat
echo.
echo To run the application:
echo   .\scripts\run.bat help
echo   .\scripts\run.bat encrypt image.jpg -p password
echo.
echo For more information, see docs/SETUP.md
echo.
echo ===================================================================
echo.

exit /b 0
