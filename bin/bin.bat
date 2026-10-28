@echo off
setlocal
for %%I in ("%~dp0..") do set "BUILD_HOME=%%~fI"
if "%~1"=="" (
	powershell.exe -NoLogo -NoProfile -NoExit -ExecutionPolicy Bypass -File "%~dp0bin.ps1"
) else (
	powershell.exe -NoLogo -NoProfile -ExecutionPolicy Bypass -File "%~dp0bin.ps1" %*
)
exit /b %errorlevel%
