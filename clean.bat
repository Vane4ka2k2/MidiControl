@echo off
cd /d "%~dp0"

echo [MidiControl] Cleaning build directory...

if exist build (
    rmdir /s /q build
    echo [MidiControl] Build directory successfully removed!
) else (
    echo [MidiControl] Build directory does not exist. Nothing to clean.
)
