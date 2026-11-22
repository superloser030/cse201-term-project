@echo off
rem Build with MSVC (C++17). Run from "Developer Command Prompt for VS" or any prompt where cl.exe is available.
rem If cl.exe is not on PATH, this script tries the default VS 2022 Build Tools location.
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
cl /nologo /std:c++17 /EHsc /W4 /permissive- /utf-8 /Fe:build\atm.exe /Fo:build\ src\*.cpp
exit /b %errorlevel%
