    @echo off
setlocal

echo ==========================================
echo      ARCHURA GAME ENGINE - PACKAGER
echo ==========================================
echo.
echo Cleaning previous build artifacts for a fresh release build...

REM Optional: Ask user if they want to clean rebuild
set /p CLEANBUILD="Clean rebuild? (Y/N, default Y): "
if /i "%CLEANBUILD%"=="" set CLEANBUILD=Y

if /i "%CLEANBUILD%"=="Y" (
    if exist build (
        echo Deleting existing build folder...
        rmdir /s /q build
    )
)

if not exist build mkdir build

cd build
echo.
echo Configuring CMake (Release)...
cmake .. -DCMAKE_BUILD_TYPE=Release
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] CMake Configuration failed.
    pause
    exit /b %ERRORLEVEL%
)

echo.
echo Building Project (Release Mode)...
cmake --build . --config Release
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Build failed.
    pause
    exit /b %ERRORLEVEL%
)

cd ..

echo.
echo Preparing Distribution Folder...
set DIST_DIR=Dist_ArchuraEngine

if exist %DIST_DIR% (
    rmdir /s /q %DIST_DIR%
)
mkdir %DIST_DIR%

REM Copy Executable and DLLs (if any) from build output
echo Copying Binaries...
xcopy /E /I /Y "build\bin\Release\*" "%DIST_DIR%\"

REM Copy Assets (Redundant if Post-Build works, but safe to ensure)
REM Check if assets were already copied by Post-Build
if not exist "%DIST_DIR%\assets" (
    echo Copying Assets manually...
    xcopy /E /I /Y "assets" "%DIST_DIR%\assets"
)

echo.
echo ==========================================
echo [SUCCESS] Game Packaged Successfully!
echo Location: %CD%\%DIST_DIR%
echo.
echo You can zip or share the '%DIST_DIR%' folder directly.
echo ==========================================
pause
