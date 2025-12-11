@echo off
echo Syncing with GitHub...
git pull origin main

echo Launching Archura Engine (Release)...
if exist "bin\Release_Dist\ArchuraEngine.exe" (
    cd bin\Release_Dist
    ArchuraEngine.exe
) else (
    echo [ERROR] Pre-compiled release not found! Please run Publish_Release.bat first or wait for update.
    pause
)
