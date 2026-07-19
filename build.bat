@echo off
setlocal enabledelayedexpansion

echo Building MidiControl project with CMake and MSVC...

:: Check if CMake is in PATH, if not add Visual Studio CMake path
where cmake >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    if exist "C:\Program Files\Microsoft Visual Studio\18\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" (
        set "PATH=C:\Program Files\Microsoft Visual Studio\18\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin;!PATH!"
    )
)

:: Initialize MSVC environment if cl is not in PATH
where cl >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    if exist "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvarsall.bat" (
        call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
    )
)

if not exist build (
    mkdir build
)

cd build
cmake ..
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] CMake configuration failed!
    exit /b %ERRORLEVEL%
)

cmake --build . --config Release
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Build failed!
    exit /b %ERRORLEVEL%
)

echo.
echo [SUCCESS] Build completed successfully!
echo Executable located at: build\Release\MidiControl.exe
