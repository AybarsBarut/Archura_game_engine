@echo off
echo ========================================
echo Archura Engine - Quick Run
echo ========================================
echo.

REM Mevcut executable'i calistir
set "EXE_PATH=bin\Release_Dist\ArchuraEngine.exe"

if exist "%EXE_PATH%" (
    echo Starting Archura Engine...
    echo.
    "%EXE_PATH%" --console
    if %ERRORLEVEL% NEQ 0 (
        echo.
        echo [ERROR] Application exited with error code %ERRORLEVEL%
        pause
    )
) else (
    echo [ERROR] Executable not found at %EXE_PATH%
    echo.
    echo Please build the project first or check the path.
    pause
)
