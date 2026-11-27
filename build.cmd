@echo off
rem ============================================================
rem  Build build\atm.exe with MSVC (C++17)
rem  Usage: double-click, or run "build.cmd" in cmd / PowerShell
rem  If cl.exe is not on PATH, the default VS 2022 location is tried.
rem ============================================================
setlocal
cd /d "%~dp0"
where cl >nul 2>nul
if errorlevel 1 (
    if exist "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat" (
        call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat" >nul
    ) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" (
        call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
    ) else (
        echo cl.exe not found. Open "Developer Command Prompt for VS" and run this script again.
        exit /b 1
    )
)
if not exist build mkdir build
cl /nologo /std:c++17 /EHsc /W4 /permissive- /utf-8 /I src ^
   /Fe:build\atm.exe /Fo:build\ ^
   src\main.cpp src\system\*.cpp src\bank\*.cpp src\atm\*.cpp src\transaction\*.cpp src\common\*.cpp
if errorlevel 1 (
    echo.
    echo BUILD FAILED
    exit /b 1
)
echo.
echo BUILD OK: build\atm.exe
exit /b 0
