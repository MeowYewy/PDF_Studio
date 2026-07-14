@echo off
setlocal
cd /d "%~dp0.."
call "%~dp0env.bat"
if errorlevel 1 exit /b 1

echo.
echo === ProjectO Release Build (v%APP_VERSION%) ===
echo Qt: %QT_DIR%
echo Output: %BUILD_EXE%
echo.

if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"

cmake -S "%PROJECT_ROOT%" -B "%BUILD_DIR%" -G "MinGW Makefiles" ^
    -DCMAKE_PREFIX_PATH="%QT_DIR%" ^
    -DCMAKE_BUILD_TYPE=Release
if errorlevel 1 exit /b 1

cmake --build "%BUILD_DIR%" --parallel
if errorlevel 1 exit /b 1

if not exist "%BUILD_EXE%" (
    echo ERROR: Build finished but %BUILD_EXE% was not created.
    exit /b 1
)

echo.
echo Build OK: %BUILD_EXE%
endlocal
