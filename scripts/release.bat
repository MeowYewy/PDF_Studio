@echo off
setlocal
cd /d "%~dp0.."

echo.
echo ========================================
echo   PageCase — Windows Release Pipeline
echo ========================================
echo.

call "%~dp0build-release.bat"
if errorlevel 1 exit /b 1

call "%~dp0deploy.bat"
if errorlevel 1 exit /b 1

call "%~dp0package-portable.bat"
if errorlevel 1 exit /b 1

call "%~dp0package-installer.bat"
if errorlevel 1 (
    echo.
    echo NOTE: Installer step skipped or failed. Portable ZIP is still available.
)

call "%~dp0env.bat"
echo.
echo ========================================
echo   Release artifacts: %ARTIFACT_DIR%
echo ========================================
dir /b "%ARTIFACT_DIR%" 2>nul
echo.
echo Next: publish to GitHub + Gitee (installer + portable):
echo   set GITHUB_TOKEN=... ^& set GITEE_TOKEN=... ^& scripts\publish-dual.bat
echo.
echo Or manually tag v%APP_VERSION% and upload from %ARTIFACT_DIR%
echo.
endlocal
