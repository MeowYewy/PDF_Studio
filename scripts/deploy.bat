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
echo === Deploy PDF Studio v%ADP_VERSION% ===
echo Staging: %DIST_DIR%
echo.

if exist "%DIST_DIR%" rmdir /s /q "%DIST_DIR%"
mkdir "%DIST_DIR%"

copy /Y "%BUILD_EXE%" "%DIST_DIR%\ProjectP.exe" >nul

pushd "%DIST_DIR%"
windeployqt --release --qmldir "%PROJECT_ROOT%\qml" --no-translations ProjectP.exe
if errorlevel 1 (
    popd
    exit /b 1
)
popd

if exist "%PROJECT_ROOT%\tools\qpdf\qpdf.exe" (
    xcopy /E /I /Y /Q "%PROJECT_ROOT%\tools\qpdf" "%DIST_DIR%\tools\qpdf" >nul
) else (
    echo WARNING: tools\qpdf not found — merge/split/rotate may fail.
)

if exist "%PROJECT_ROOT%\tools\poppler\pdftoppm.exe" (
    xcopy /E /I /Y /Q "%PROJECT_ROOT%\tools\poppler" "%DIST_DIR%\tools\poppler" >nul
) else (
    echo NOTE: tools\poppler not found — install Qt Pdf or run setup-poppler.bat.
)

copy /Y "%PROJECT_ROOT%\LICENSE" "%DIST_DIR%\LICENSE.txt" >nul
copy /Y "%PROJECT_ROOT%\packaging\windows\third-party-LICENSES.txt" "%DIST_DIR%\third-party-LICENSES.txt" >nul

echo.
echo Deploy OK: %DIST_DIR%
echo Run: "%DIST_DIR%\ProjectP.exe"
endlocal
