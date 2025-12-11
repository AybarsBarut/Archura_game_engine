@echo off
echo Building Archura Engine...

if not exist build mkdir build
cd build
cmake ..
if %errorlevel% neq 0 (
    echo CMake Configuration failed!
    pause
    exit /b %errorlevel%
)

cmake --build . --config Release
if %errorlevel% neq 0 (
    echo Build failed!
    pause
    exit /b %errorlevel%
)

cd ..
echo Starting Game...
build\bin\Release\ArchuraEngine.exe
