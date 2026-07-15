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
echo Next: tag v%APP_VERSION% and upload to GitHub Releases.
echo   gh release create v%APP_VERSION% ^
echo     "%ARTIFACT_DIR%\PageCase_%APP_VERSION%_win64_portable.zip" ^
echo     "%ARTIFACT_DIR%\PageCase_%APP_VERSION%_win64_Setup.exe" ^
echo     --title "PageCase v%APP_VERSION%" ^
echo     --notes-file "%PROJECT_ROOT%\resources\changelog.json"
echo.
endlocal
