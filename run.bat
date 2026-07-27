@echo off
setlocal enabledelayedexpansion

cd /d "%~dp0"

if not exist "build\Release\MidiControl.exe" (
    if exist "build\Debug\MidiControl.exe" (
        echo [+] Launching Debug build...
        "build\Debug\MidiControl.exe"
        exit /b 0
    )
    echo [*] Binary not found. Running build.bat first...
    call build.bat
    if errorlevel 1 exit /b 1
)

echo [+] Launching MidiControl.exe...
"build\Release\MidiControl.exe"
