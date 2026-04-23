@echo off
REM ZBW POS Windows Build Script
REM Requires: Qt 5.15 (MSVC 2019) or Qt 6.x, Visual Studio 2019+

setlocal enabledelayedexpansion

REM Configuration
set "APP_NAME=zbwpos"
set "APP_VERSION=1.0.0"
set "BUILD_DIR=build-windows"
set "INSTALL_DIR=install"
set "PACKAGE_DIR=package"

REM Qt paths - adjust these for your installation
if "%QT_DIR%"=="" (
    set "QT_DIR=C:\Qt\5.15.2\msvc2019_64"
    if not exist "!QT_DIR!" set "QT_DIR=C:\Qt\6.5.0\msvc2019_64"
)

set "PATH=%QT_DIR%\bin;%PATH%"

echo ========================================
echo ZBW POS Windows Build
echo Qt: %QT_DIR%
echo ========================================

REM Setup MSVC environment
where cl >nul 2>&1
if errorlevel 1 (
    echo Setting up MSVC environment...
    call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" 2>nul
    if errorlevel 1 (
        call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat" 2>nul
    )
)

if not exist "%QT_DIR%\bin\qmake.exe" (
    echo ERROR: Qt not found at %QT_DIR%
    echo Please set QT_DIR environment variable or install Qt
    exit /b 1
)

REM Clean previous build
if exist "%BUILD_DIR%" rmdir /s /q "%BUILD_DIR%"
mkdir "%BUILD_DIR%"

REM Build
cd "%BUILD_DIR%"
echo Running qmake...
"%QT_DIR%\bin\qmake.exe" ../zbwpos.pro -spec win32-msvc CONFIG+=release

echo Building...
nmake

if errorlevel 1 (
    echo Build failed!
    exit /b 1
)

REM Install
echo Installing to %INSTALL_DIR%...
if exist "..\%INSTALL_DIR%" rmdir /s /q "..\%INSTALL_DIR%"
mkdir "..\%INSTALL_DIR%\bin"

copy "src\app\release\%APP_NAME%.exe" "..\%INSTALL_DIR%\bin\"

REM Deploy Qt dependencies
echo Deploying Qt dependencies...
cd "..\%INSTALL_DIR%\bin"
"%QT_DIR%\bin\windeployqt.exe" --release --no-translations "%APP_NAME%.exe"

REM Copy additional files
if exist "..\..\resources" xcopy /s /y "..\..\resources" "..\%INSTALL_DIR%\"

cd ..\..

echo ========================================
echo Build complete: %INSTALL_DIR%\bin\%APP_NAME%.exe
echo ========================================

REM Create NSIS installer if available
where makensis >nul 2>&1
if not errorlevel 1 (
    echo Creating installer...
    makensis /DVERSION=%APP_VERSION% scripts\installer-windows.nsi
)

endlocal
