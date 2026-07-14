@echo off
setlocal EnableDelayedExpansion
cd /d "%~dp0.."
call "%~dp0env.bat"
if errorlevel 1 exit /b 1

if not exist "%BUILD_EXE%" (
    echo ERROR: %BUILD_EXE% not found. Run scripts\build-release.bat first.
    exit /b 1
)

echo.
echo === Deploy ProjectO v%APP_VERSION% ===
echo Staging: %DIST_DIR%
echo.

if exist "%DIST_DIR%" rmdir /s /q "%DIST_DIR%"
mkdir "%DIST_DIR%"

copy /Y "%BUILD_EXE%" "%DIST_DIR%\ProjectO.exe" >nul

pushd "%DIST_DIR%"
windeployqt --release --qmldir "%PROJECT_ROOT%\qml" --no-translations ProjectO.exe
if errorlevel 1 (
    popd
    exit /b 1
)
popd

if exist "%PROJECT_ROOT%\tools\poppler\pdftoppm.exe" (
    xcopy /E /I /Y /Q "%PROJECT_ROOT%\tools\poppler" "%DIST_DIR%\tools\poppler" >nul
) else (
    echo NOTE: tools\poppler not found — install Qt Pdf module or run setup-poppler.bat.
)

copy /Y "%PROJECT_ROOT%\LICENSE" "%DIST_DIR%\LICENSE.txt" >nul
if exist "%PROJECT_ROOT%\packaging\windows\third-party-LICENSES.txt" (
    copy /Y "%PROJECT_ROOT%\packaging\windows\third-party-LICENSES.txt" "%DIST_DIR%\third-party-LICENSES.txt" >nul
)

echo.
echo Deploy OK: %DIST_DIR%
echo Run: "%DIST_DIR%\ProjectO.exe"
endlocal
