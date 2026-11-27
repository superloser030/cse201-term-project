@echo off
rem ============================================================
rem  Run the ATM program (builds first if build\atm.exe is missing)
rem  Runs in the project root so sample_initial_condition.txt is found.
rem ============================================================
setlocal
cd /d "%~dp0"
if not exist build\atm.exe (
    call build.cmd
    if errorlevel 1 exit /b 1
)
chcp 65001 >nul
build\atm.exe
