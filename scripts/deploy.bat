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
echo === Deploy PageCase v%APP_VERSION% ===
echo Staging: %DIST_DIR%
echo.

if exist "%DIST_DIR%" rmdir /s /q "%DIST_DIR%"
mkdir "%DIST_DIR%"

copy /Y "%BUILD_EXE%" "%DIST_DIR%\PageCase.exe" >nul

pushd "%DIST_DIR%"
windeployqt --release --qmldir "%PROJECT_ROOT%\qml" --no-translations PageCase.exe
if errorlevel 1 (
    popd
    exit /b 1
)
popd

rem MinGW runtimes — required on machines without Qt/MinGW in PATH (silent fail otherwise)
for %%F in (libgcc_s_seh-1.dll libstdc++-6.dll libwinpthread-1.dll) do (
    if exist "%MINGW_DIR%\bin\%%F" (
        copy /Y "%MINGW_DIR%\bin\%%F" "%DIST_DIR%\%%F" >nul
    ) else if exist "%QT_DIR%\bin\%%F" (
        copy /Y "%QT_DIR%\bin\%%F" "%DIST_DIR%\%%F" >nul
    )
)

rem Software OpenGL fallback for PCs without working GPU drivers
if exist "%QT_DIR%\bin\opengl32sw.dll" (
    copy /Y "%QT_DIR%\bin\opengl32sw.dll" "%DIST_DIR%\opengl32sw.dll" >nul
)

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
echo Run: "%DIST_DIR%\PageCase.exe"
endlocal
