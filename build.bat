@echo off
setlocal enabledelayedexpansion

cd /d "%~dp0"

set "CMAKE_EXE=C:\Program Files\Microsoft Visual Studio\18\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"

if not exist "!CMAKE_EXE!" (
    where cmake >nul 2>nul
    if !errorlevel! equ 0 (
        set "CMAKE_EXE=cmake"
    ) else (
        echo [!] CMake was not found. Please install CMake or Visual Studio C++ workload.
        exit /b 1
    )
)

echo [+] Configuring and building MidiControl...
if not exist build mkdir build

echo [+] Running CMake Configuration...
"!CMAKE_EXE!" -S . -B build -G "Visual Studio 18 2026" -A x64
if errorlevel 1 (
    echo [!] CMake configuration failed.
    exit /b 1
)

echo [+] Compiling Release build...
"!CMAKE_EXE!" --build build --config Release
if errorlevel 1 (
    echo [!] Compilation failed.
    exit /b 1
)

echo.
echo [x] Build completed successfully! Binary: build\Release\MidiControl.exe
