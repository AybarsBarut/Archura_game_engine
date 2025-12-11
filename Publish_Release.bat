@echo off
echo ==========================================
echo      ARCHURA ENGINE - RELEASE PUBLISHER
echo ==========================================

echo 1. Building Release Config...
cmake --build build --config Release

if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Build failed! Aborting publish.
    pause
    exit /b %ERRORLEVEL%
)

echo 2. Ensuring Release_Dist folder exists...
if not exist "bin\Release_Dist" mkdir bin\Release_Dist

echo 3. Copying Binaries...
copy /Y "build\bin\Release\ArchuraEngine.exe" "bin\Release_Dist\"
copy /Y "build\bin\Release\*.dll" "bin\Release_Dist\" 2>nul
echo Copying Assets...
xcopy /E /I /Y "assets" "bin\Release_Dist\assets"

echo 4. Committing to Git...
echo Adding all source changes...
git add .
echo Force adding Release binaries...
git add -f bin/Release_Dist
git commit -m "Release Update: %DATE% %TIME%"
echo Pushing to GitHub...
git push origin main

echo ==========================================
echo      PUBLISH COMPLETE!
echo ==========================================
pause
