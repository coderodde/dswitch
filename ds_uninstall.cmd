@echo off
setlocal EnableExtensions EnableDelayedExpansion

set "DSDIR=%USERPROFILE%\.dswitcher"

REM Make sure we are not inside the directory we want to delete
cd /d "%USERPROFILE%" >nul 2>&1

REM Remove folder tree
if exist "%DSDIR%" (
  rmdir /S /Q "%DSDIR%" >nul 2>&1
)

REM If still exists, show why (optional diagnostics)
if exist "%DSDIR%" (
  echo Failed to remove "%DSDIR%".
  echo Possible reasons: file is in use, permissions, or current directory.
  echo Contents left:
  dir /a "%DSDIR%"
)

reg delete "HKCU\Software\Microsoft\Command Processor" /v AutoRun /f

echo Done.
endlocal
exit /b 0
