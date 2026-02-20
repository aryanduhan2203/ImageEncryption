# ImageEncryption - Setup Guide

Complete guide for building and running ImageEncryption on Windows, macOS, and Linux.

## Table of Contents

- [Quick Start](#quick-start)
- [System Requirements](#system-requirements)
- [Installation](#installation)
  - [Windows](#windows)
  - [macOS](#macos)
  - [Linux](#linux)
- [Building the Project](#building-the-project)
- [Running the Application](#running-the-application)
- [Troubleshooting](#troubleshooting)

---

## Quick Start

```bash
# Windows
.\scripts\setup_env.bat          # Check environment
.\scripts\build_fast.bat         # Build
.\scripts\run.bat encrypt image.jpg -p "password"

# macOS/Linux
./scripts/build.sh               # Build
./build/imgenc encrypt image.jpg -p "password"
```

---

## System Requirements

### Mandatory
- **CMake** 3.15 or higher
- **C++ compiler** with C++17 support:
  - Windows: Visual Studio 2019+ or Visual Studio Build Tools 2019+
  - macOS: Clang (included with Xcode)
  - Linux: GCC 7.0+ or Clang 5.0+
- **OpenSSL** 1.1.1 or higher
- **OpenCV** 4.5 or higher

### Optional
- **Git** (for version control)
- **GoogleTest** (for running tests)
- **Python 3.7+** (for utility scripts)

---

## Installation

### Windows

#### 1. Install Visual Studio Build Tools

Option A: Full Visual Studio 2019/2022
```
https://visualstudio.microsoft.com/downloads/
```

Option B: Build Tools only (lighter weight)
```
https://visualstudio.microsoft.com/visual-cpp-build-tools/
```

Ensure C++ workload is selected during installation.

#### 2. Install CMake

Download from: https://cmake.org/download/

```powershell
# Using Chocolatey (if installed)
choco install cmake

# Or download MSI installer from cmake.org and run
```

Verify installation:
```powershell
cmake --version
```

#### 3. Install OpenSSL

Option A: Using Chocolatey
```powershell
choco install openssl
```

Option B: Download pre-built binaries
```
https://slproweb.com/products/Win32OpenSSL.html
```

Option C: Build from source
```powershell
# Clone the repository
git clone https://github.com/openssl/openssl.git
cd openssl

# Configure and build
perl Configure VC-WIN64A
nmake
nmake install
```

#### 4. Install OpenCV

Option A: Using pre-built binaries
```
Download from: https://opencv.org/releases/
Extract to: C:\opencv
```

Option B: Build from source
```powershell
git clone https://github.com/opencv/opencv.git
cd opencv
mkdir build
cd build
cmake -G "Visual Studio 16 2019" -A x64 -DCMAKE_INSTALL_PREFIX=C:\opencv ..
cmake --build . --config Release
cmake --install . --config Release
```

#### 5. Configure Environment Variables

Add the following to your system PATH:
```
C:\opencv\bin
C:\opencv\lib
C:\Program Files\OpenSSL\bin
C:\Program Files\OpenSSL\lib
```

To set environment variables:
1. Open Settings → Environment Variables
2. Click "Edit the system environment variables"
3. Click "Environment Variables..."
4. Add the paths to the PATH variable

Or via PowerShell:
```powershell
[Environment]::SetEnvironmentVariable("PATH", [Environment]::GetEnvironmentVariable("PATH") + ";C:\opencv\bin;C:\Program Files\OpenSSL\bin", "Machine")
```

---

### macOS

#### 1. Install Xcode Command Line Tools

```bash
xcode-select --install
```

#### 2. Install Homebrew

```bash
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

#### 3. Install Dependencies

```bash
brew install cmake openssl opencv
```

Verify installation:
```bash
cmake --version
openssl version
```

---

### Linux

#### Ubuntu/Debian

```bash
# Update package lists
sudo apt-get update

# Install dependencies
sudo apt-get install -y \
    build-essential \
    cmake \
    libssl-dev \
    libopencv-dev \
    git

# Verify installation
cmake --version
openssl version
pkg-config --modversion opencv
```

#### Red Hat/CentOS/Fedora

```bash
# Install dependencies
sudo yum groupinstall -y "Development Tools"
sudo yum install -y cmake openssl-devel opencv-devel git

# Verify installation
cmake --version
openssl version
ldconfig -v | grep opencv
```

#### Arch Linux

```bash
# Install dependencies
sudo pacman -S base-devel cmake openssl opencv git

# Verify installation
cmake --version
openssl version
```

---

## Building the Project

### Windows

#### Using Batch Scripts (Recommended)

```powershell
# Check environment setup
.\scripts\setup_env.bat

# Build the project
.\scripts\build_fast.bat

# Clean build artifacts
.\scripts\clean.bat
```

#### Manual CMake Build

```powershell
# Create and configure build directory
mkdir build
cd build
cmake -G "Visual Studio 16 2019" -A x64 ..

# Build
cmake --build . --config Release

# Built executable will be at: build\Release\imgenc.exe
```

### macOS/Linux

#### Using Bash Scripts

```bash
# Make scripts executable
chmod +x scripts/*.sh

# Build the project
./scripts/build.sh

# Clean build artifacts
./scripts/clean.sh
```

#### Manual CMake Build

```bash
# Create and configure build directory
mkdir build
cd build
cmake ..

# Build
cmake --build . --config Release

# Built executable will be at: build/imgenc
```

---

## Running the Application

### Basic Usage

#### Windows

```powershell
# Using script
.\scripts\run.bat encrypt image.jpg -p "MyPassword"
.\scripts\run.bat decrypt image.enc -p "MyPassword" -o decrypted.jpg

# Using executable directly
.\build\Release\imgenc.exe encrypt image.jpg -p "MyPassword"
```

#### macOS/Linux

```bash
# Using script
./scripts/run.sh encrypt image.jpg -p "MyPassword"
./scripts/run.sh decrypt image.enc -p "MyPassword" -o decrypted.jpg

# Using executable directly
./build/imgenc encrypt image.jpg -p "MyPassword"
```

### Command Syntax

```
Usage: imgenc <command> [options]

Commands:
  encrypt <input>     Encrypt an image file
  decrypt <input>     Decrypt an encrypted image
  help                Show this help message

Options:
  -o, --output PATH   Output file path (auto-generated if not provided)
  -p, --password PASS Encryption/decryption password
  -r, --r VALUE       Chaotic map parameter (default: 3.99, range: 3.57-4.0)

Examples:
  # Encrypt with password prompt
  imgenc encrypt photo.jpg
  
  # Encrypt with password argument
  imgenc encrypt photo.jpg -p "SecurePassword123"
  
  # Encrypt with custom output
  imgenc encrypt photo.jpg -p "SecurePassword123" -o photo_encrypted.enc
  
  # Decrypt
  imgenc decrypt photo.enc -p "SecurePassword123" -o photo_decrypted.jpg
  
  # Decrypt with custom chaotic parameter
  imgenc decrypt photo.enc -p "SecurePassword123" -r 3.99
```

### Password Input

If you don't provide a password with `-p`, the application will prompt you to enter it:

```
Enter encryption password: (hidden input)
Enter decryption password: (hidden input)
```

---

## Troubleshooting

### CMake Configuration Issues

**Problem**: `CMake Error: Could not find OpenSSL`

**Solution**:
```bash
# On Windows, specify OpenSSL path
cmake -DOPENSSL_ROOT_DIR="C:\Program Files\OpenSSL" ..

# On macOS
cmake -DOPENSSL_ROOT_DIR=$(brew --prefix openssl) ..
```

**Problem**: `CMake Error: Could not find OpenCV`

**Solution**:
```bash
# On Windows
cmake -DCMAKE_PREFIX_PATH="C:\opencv" ..

# On macOS
cmake -DCMAKE_PREFIX_PATH=$(brew --prefix opencv) ..

# On Linux (if using non-standard installation)
cmake -DCMAKE_PREFIX_PATH=/usr/local/opencv ..
```

### Build Errors

**Problem**: "Visual Studio not found" (Windows)

**Solution**:
Specify the generator explicitly:
```powershell
cmake -G "Visual Studio 16 2019" -A x64 ..
```

Available generators:
- Visual Studio 16 2019 (VS 2019)
- Visual Studio 17 2022 (VS 2022)
- Ninja
- Unix Makefiles

**Problem**: "Permission denied" (macOS/Linux)

**Solution**:
Make scripts executable:
```bash
chmod +x scripts/*.sh
./scripts/build.sh
```

### Runtime Errors

**Problem**: `Error: Library not found` or `DLL not found`

**Solution**:
Windows:
- Add DLL paths to PATH environment variable
- Copy DLLs to project root or build/Release directory

macOS:
```bash
# Set library path
export DYLD_LIBRARY_PATH=/usr/local/lib:$DYLD_LIBRARY_PATH
```

Linux:
```bash
export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
```

**Problem**: `Error: Failed to load image`

**Solution**:
- Verify image file exists and is readable
- Check image format is supported (JPG, PNG, BMP, etc.)
- Use absolute path if relative path doesn't work

**Problem**: `Error: Wrong password or corrupted file`

**Solution**:
- Verify password is correct (case-sensitive)
- Ensure decrypting the correct .enc file
- Check file wasn't corrupted during transfer

### Environment Variables

To set temporary environment variables:

**Windows (PowerShell)**:
```powershell
$env:PATH += ";C:\opencv\bin;C:\Program Files\OpenSSL\bin"
```

**Windows (CMD)**:
```cmd
set PATH=%PATH%;C:\opencv\bin;C:\Program Files\OpenSSL\bin
```

**macOS/Linux**:
```bash
export PATH=/usr/local/opencv/bin:$PATH
export LD_LIBRARY_PATH=/usr/local/opencv/lib:$LD_LIBRARY_PATH
```

### Getting Help

If issues persist:

1. Check CMake version: `cmake --version`
2. Verify OpenSSL: `openssl version`
3. Verify OpenCV: `pkg-config --modversion opencv` (Linux) or check installation directory
4. Try clean build: `./scripts/clean.bat` then `./scripts/build_fast.bat`
5. Check build logs in the `build/` directory

---

## Advanced Configuration

### Custom CMake Options

```bash
# Enable verbose output
cmake --build . --verbose

# Build specific config
cmake --build . --config Debug

# Install to custom location
cmake --install . --prefix /custom/path
```

### Environment-Specific Builds

Set build type during configuration:
```bash
# Debug build
cmake -DCMAKE_BUILD_TYPE=Debug ..

# Release build (smaller, faster)
cmake -DCMAKE_BUILD_TYPE=Release ..
```

---

## Performance Optimization

For maximum performance:

1. **Build in Release mode** (default in scripts):
   ```bash
   cmake -DCMAKE_BUILD_TYPE=Release ..
   ```

2. **Use latest OpenCV** with hardware acceleration
3. **Run on SSD** for faster I/O with large images
4. **Consider image size**: processing 4K images will take longer than 1080p

---

## Security Notes

- **Password strength**: Use strong, unique passwords (20+ characters recommended)
- **Password storage**: Never hardcode passwords in scripts
- **File permissions**: Protect encrypted files appropriately
- **OpenSSL updates**: Keep OpenSSL updated for security patches

---

For more information, see the main [README.md](../README.md) in the project root.
