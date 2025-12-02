@echo off
echo ========================================
echo Archura Engine - Dependency Setup
echo ========================================
echo.

REM Git submodule'lerini kontrol et veya indir
echo [1/4] Checking git submodules...
if not exist "external\.git" (
    git init external
)

REM GLFW
echo [2/4] Setting up GLFW...
if not exist "external\glfw" (
    echo Cloning GLFW...
    git clone --depth 1 --branch 3.3.8 https://github.com/glfw/glfw.git external/glfw
) else (
    echo GLFW already exists.
)

REM GLM
echo [3/4] Setting up GLM...
if not exist "external\glm" (
    echo Cloning GLM...
    git clone --depth 1 --branch 0.9.9.8 https://github.com/g-truc/glm.git external/glm
) else (
    echo GLM already exists.
)

REM ImGui
echo [4/5] Setting up Dear ImGui...
if not exist "external\imgui" (
    echo Cloning ImGui...
    git clone --depth 1 --branch v1.89.9 https://github.com/ocornut/imgui.git external/imgui
) else (
    echo ImGui already exists.
)

REM stb_image
echo [5/5] Setting up stb_image...
if not exist "external\stb" mkdir external\stb
if not exist "external\stb\stb_image.h" (
    echo Downloading stb_image.h...
    powershell -Command "Invoke-WebRequest -Uri 'https://raw.githubusercontent.com/nothings/stb/master/stb_image.h' -OutFile 'external/stb/stb_image.h'"
    if errorlevel 1 (
        echo Warning: Failed to download stb_image.h automatically
        echo Please download manually from: https://raw.githubusercontent.com/nothings/stb/master/stb_image.h
    )
) else (
    echo stb_image.h already exists.
)

REM GLAD dosyalarını kontrol et
if not exist "external\glad\include" (
    echo.
    echo WARNING: GLAD files not found!
    echo Please generate GLAD files from: https://glad.dav1d.de/
    echo - Language: C/C++
    echo - Specification: OpenGL
    echo - gl Version: 3.3+
    echo - Profile: Core
    echo - Generate a loader: YES
    echo.
    echo Extract to: external/glad/
    echo.
)

echo.
echo ========================================
echo Dependencies setup complete!
echo ========================================
echo.
echo Next steps:
echo 1. Generate GLAD files if needed (see above)
echo 2. mkdir build ^&^& cd build
echo 3. cmake ..
echo 4. cmake --build . --config Release
echo.
pause
