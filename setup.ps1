# Archura Engine - Dependency Setup Script

Write-Host "========================================"  -ForegroundColor Cyan
Write-Host "Archura Engine - Dependency Setup" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# External dizinini oluştur
if (!(Test-Path "external")) {
    New-Item -ItemType Directory -Path "external" | Out-Null
}

# GLFW
Write-Host "[1/4] Setting up GLFW..." -ForegroundColor Yellow
if (!(Test-Path "external\glfw")) {
    Write-Host "Cloning GLFW..." -ForegroundColor Green
    git clone --depth 1 --branch 3.3.8 https://github.com/glfw/glfw.git external/glfw
}
else {
    Write-Host "GLFW already exists." -ForegroundColor Gray
}

# GLM
Write-Host "[2/4] Setting up GLM..." -ForegroundColor Yellow
if (!(Test-Path "external\glm")) {
    Write-Host "Cloning GLM..." -ForegroundColor Green
    git clone --depth 1 --branch 0.9.9.8 https://github.com/g-truc/glm.git external/glm
}
else {
    Write-Host "GLM already exists." -ForegroundColor Gray
}

# ImGui
Write-Host "[3/5] Setting up Dear ImGui..." -ForegroundColor Yellow
if (!(Test-Path "external\imgui")) {
    Write-Host "Cloning ImGui..." -ForegroundColor Green
    git clone --depth 1 --branch v1.89.9 https://github.com/ocornut/imgui.git external/imgui
}
else {
    Write-Host "ImGui already exists." -ForegroundColor Gray
}

# stb_image
Write-Host "[4/5] Setting up stb_image..." -ForegroundColor Yellow
if (!(Test-Path "external\stb")) {
    New-Item -ItemType Directory -Path "external\stb" | Out-Null
}
if (!(Test-Path "external\stb\stb_image.h")) {
    Write-Host "Downloading stb_image.h..." -ForegroundColor Green
    Invoke-WebRequest -Uri "https://raw.githubusercontent.com/nothings/stb/master/stb_image.h" -OutFile "external/stb/stb_image.h"
}
else {
    Write-Host "stb_image.h already exists." -ForegroundColor Gray
}

# GLAD kontrolü
Write-Host "[5/5] Checking GLAD..." -ForegroundColor Yellow
if (!(Test-Path "external\glad\include")) {
    Write-Host ""
    Write-Host "WARNING: GLAD files not found!" -ForegroundColor Red
    Write-Host "Please generate GLAD files from: https://glad.dav1d.de/" -ForegroundColor Yellow
    Write-Host "  - Language: C/C++" -ForegroundColor White
    Write-Host "  - Specification: OpenGL" -ForegroundColor White
    Write-Host "  - gl Version: 3.3+" -ForegroundColor White
    Write-Host "  - Profile: Core" -ForegroundColor White
    Write-Host "  - Generate a loader: YES" -ForegroundColor White
    Write-Host ""
    Write-Host "Extract to: external/glad/" -ForegroundColor Yellow
    Write-Host ""
}
else {
    Write-Host "GLAD files found." -ForegroundColor Gray
}

Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Dependencies setup complete!" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "Next steps:" -ForegroundColor Yellow
Write-Host "1. Generate GLAD files if needed (see above)" -ForegroundColor White
Write-Host "2. mkdir build && cd build" -ForegroundColor White
Write-Host "3. cmake .." -ForegroundColor White
Write-Host "4. cmake --build . --config Release" -ForegroundColor White
Write-Host ""
