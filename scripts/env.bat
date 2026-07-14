@echo off
setlocal EnableDelayedExpansion

rem Resolve project root (desktop-qt/)
set "PROJECT_ROOT=%~dp0.."
for %%I in ("%PROJECT_ROOT%") do set "PROJECT_ROOT=%%~fI"

rem Read version from VERSION file
set "APP_VERSION=0.1.0"
if exist "%PROJECT_ROOT%\APP_VERSION.txt" (
    for /f "usebackq delims=" %%V in ("%PROJECT_ROOT%\APP_VERSION.txt") do set "APP_VERSION=%%V"
)

rem Qt / MinGW / CMake — override with QT_DIR, MINGW_DIR, CMAKE_DIR if needed
if not defined QT_DIR (
    if defined QT_ROOT_DIR (
        set "QT_DIR=%QT_ROOT_DIR%"
    ) else if defined QTDIR (
        set "QT_DIR=%QTDIR%"
    ) else if exist "D:\Qt\6.11.1\mingw_64\bin\qmake.exe" (
        set "QT_DIR=D:\Qt\6.11.1\mingw_64"
    ) else if exist "C:\Qt\6.11.1\mingw_64\bin\qmake.exe" (
        set "QT_DIR=C:\Qt\6.11.1\mingw_64"
    )
)

if not defined MINGW_DIR (
    if defined IQTA_TOOLS (
        if exist "%IQTA_TOOLS%\mingw1310_64\bin\g++.exe" (
            set "MINGW_DIR=%IQTA_TOOLS%\mingw1310_64"
        )
    )
)
if not defined MINGW_DIR (
    if exist "D:\Qt\Tools\mingw1310_64\bin\g++.exe" (
        set "MINGW_DIR=D:\Qt\Tools\mingw1310_64"
    ) else if exist "C:\Qt\Tools\mingw1310_64\bin\g++.exe" (
        set "MINGW_DIR=C:\Qt\Tools\mingw1310_64"
    )
)
if not defined MINGW_DIR (
    if defined QT_DIR (
        for /d %%D in ("%QT_DIR%\..\..\Tools\mingw*") do (
            if exist "%%D\bin\g++.exe" set "MINGW_DIR=%%D"
        )
    )
)

if not defined CMAKE_DIR (
    if exist "D:\Qt\Tools\CMake_64\bin\cmake.exe" (
        set "CMAKE_DIR=D:\Qt\Tools\CMake_64\bin"
    ) else if exist "C:\Qt\Tools\CMake_64\bin\cmake.exe" (
        set "CMAKE_DIR=C:\Qt\Tools\CMake_64\bin"
    )
)
rem GitHub Actions / system CMake
if not defined CMAKE_DIR (
    where cmake >nul 2>&1
    if not errorlevel 1 set "CMAKE_DIR="
)

if not defined QT_DIR (
    echo [env.bat] ERROR: Qt not found. Set QT_DIR or install Qt 6.11.1 MinGW 64-bit.
    exit /b 1
)

if not exist "%QT_DIR%\bin\windeployqt.exe" (
    echo [env.bat] ERROR: windeployqt not found in %QT_DIR%\bin
    exit /b 1
)

set "PATH=%QT_DIR%\bin;%MINGW_DIR%\bin;%CMAKE_DIR%;%PATH%"

set "BUILD_DIR=%PROJECT_ROOT%\build\release"
set "BUILD_EXE=%BUILD_DIR%\ProjectO.exe"
set "DIST_ROOT=%PROJECT_ROOT%\dist"
rem Artifact naming: ProjectO_{version}_win64
set "DIST_DIR=%DIST_ROOT%\ProjectO_%APP_VERSION%_win64"
set "ARTIFACT_DIR=%DIST_ROOT%\artifacts"

endlocal & (
    set "PROJECT_ROOT=%PROJECT_ROOT%"
    set "APP_VERSION=%APP_VERSION%"
    set "QT_DIR=%QT_DIR%"
    set "MINGW_DIR=%MINGW_DIR%"
    set "CMAKE_DIR=%CMAKE_DIR%"
    set "BUILD_DIR=%BUILD_DIR%"
    set "BUILD_EXE=%BUILD_EXE%"
    set "DIST_ROOT=%DIST_ROOT%"
    set "DIST_DIR=%DIST_DIR%"
    set "ARTIFACT_DIR=%ARTIFACT_DIR%"
    set "PATH=%PATH%"
)
