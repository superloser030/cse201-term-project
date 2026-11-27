@echo off
rem ============================================================
rem  Run automated tests (builds first if build\atm.exe is missing)
rem  Usage:  test.cmd          run all tests
rem          test.cmd 23       run tests whose name contains "23"
rem ============================================================
setlocal
cd /d "%~dp0"
if not exist build\atm.exe (
    call build.cmd
    if errorlevel 1 exit /b 1
)
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0test\run_tests.ps1" %*
exit /b %errorlevel%
