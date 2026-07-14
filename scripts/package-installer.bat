@echo off
setlocal EnableDelayedExpansion
cd /d "%~dp0.."
call "%~dp0env.bat"
if errorlevel 1 exit /b 1

if not exist "%DIST_DIR%\ProjectO.exe" (
    echo ERROR: %DIST_DIR% not found. Run scripts\deploy.bat first.
    exit /b 1
)

set "ISCC="
if defined INNO_SETUP_DIR (
    if exist "%INNO_SETUP_DIR%\ISCC.exe" set "ISCC=%INNO_SETUP_DIR%\ISCC.exe"
)
if not defined ISCC if exist "D:\Inno Setup 6\ISCC.exe" set "ISCC=D:\Inno Setup 6\ISCC.exe"
if not defined ISCC if exist "C:\Program Files (x86)\Inno Setup 6\ISCC.exe" set "ISCC=C:\Program Files (x86)\Inno Setup 6\ISCC.exe"
if not defined ISCC if exist "C:\Program Files\Inno Setup 6\ISCC.exe" set "ISCC=C:\Program Files\Inno Setup 6\ISCC.exe"

if not defined ISCC (
    echo ERROR: Inno Setup 6 not found.
    echo Download: https://jrsoftware.org/isdl.php
    echo Then run this script again, or open packaging\windows\ProjectO.iss manually.
    exit /b 1
)

if not exist "%ARTIFACT_DIR%" mkdir "%ARTIFACT_DIR%"

echo.
echo === Build installer with Inno Setup ===
echo.

"%ISCC%" /DAppVersion=%APP_VERSION% /DSourceDir="%DIST_DIR%" /DOutputDir="%ARTIFACT_DIR%" "%PROJECT_ROOT%\packaging\windows\ProjectO.iss"
if errorlevel 1 exit /b 1

echo.
echo Installer OK: %ARTIFACT_DIR%\ProjectO_%APP_VERSION%_win64_Setup.exe
endlocal
