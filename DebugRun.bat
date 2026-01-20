@echo off
echo Starting Archura Game Engine with Debug Console...
if exist "build\bin\Release\ArchuraLauncher.exe" (
    "build\bin\Release\ArchuraLauncher.exe" --console
) else (
    if exist "dist\ArchuraLauncher.exe" (
        "dist\ArchuraLauncher.exe" --console
    ) else (
        echo Error: Could not find ArchuraLauncher.exe in build\bin\Release or dist
        echo Please build the project first.
        pause
    )
)
