# Archura Engine - Project Packager
# Exports clean source code for distribution/testing

$sourceDir = Get-Location
$distDir = "$sourceDir\dist\Archura_v1.0.0"

Write-Host "Packaging Archura Engine..." -ForegroundColor Cyan
Write-Host "Source: $sourceDir"
Write-Host "Destination: $distDir"
Write-Host ""

# Clean previous dist
if (Test-Path $distDir) {
    Remove-Item -Path $distDir -Recurse -Force
}
New-Item -ItemType Directory -Path $distDir -Force | Out-Null

# Define files/folders to copy
$includes = @(
    "src",
    "assets",
    "external",
    "CMakeLists.txt",
    "setup.ps1",
    "version.txt"
)

foreach ($item in $includes) {
    $srcPath = Join-Path $sourceDir $item
    $destPath = Join-Path $distDir $item

    if (Test-Path $srcPath) {
        Write-Host "Copying $item..." -ForegroundColor Yellow
        if (Test-Path $srcPath -PathType Container) {
            Copy-Item -Path $srcPath -Destination $distDir -Recurse -Force
        }
        else {
            Copy-Item -Path $srcPath -Destination $distDir -Force
        }
    }
    else {
        Write-Host "Warning: $item not found!" -ForegroundColor Red
    }
}

# Create a Readme for the dist
$readmeContent = @"
# Archura Game Engine v1.0.0

## Installation
1. Run 'setup.ps1' to verify dependencies (requires PowerShell).
2. Open a terminal in this directory.
3. Run the following commands:

    mkdir build
    cd build
    cmake ..
    cmake --build . --config Release

4. Run the engine:
    ./bin/Release/ArchuraLauncher.exe
"@
Set-Content -Path "$distDir\README_DIST.txt" -Value $readmeContent

Write-Host ""
Write-Host "Packaging Complete!" -ForegroundColor Green
Write-Host "Output: $distDir"
Write-Host "You can now zip this folder or move it to test extraction." -ForegroundColor Gray
