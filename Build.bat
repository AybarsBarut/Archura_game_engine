@echo off
setlocal

echo ==========================================
echo      Archura Game Engine Build Script
echo ==========================================

if not exist build (
    echo [INFO] Build directory not found. Creating...
    mkdir build
)

echo [INFO] Configuring CMake...
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] CMake configuration failed!
    pause
    exit /b %ERRORLEVEL%
)

echo [INFO] Building project...
cmake --build build --config Release
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Build failed!
    pause
    exit /b %ERRORLEVEL%
)

echo.
echo [SUCCESS] Build completed successfully!
echo You can run the engine using StartGame_Release.bat
echo.
pause
