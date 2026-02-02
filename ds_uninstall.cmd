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

REM 2) Remove AutoRun value IF it matches what we installed
set "REGKEY=HKCU\Software\Microsoft\Command Processor"
set "REGVAL=AutoRun"

for /f "skip=2 tokens=1,2,*" %%A in ('reg query "%REGKEY%" /v "%REGVAL%" 2^>nul') do (
  set "CUR_AUTORUN=%%C"
)

if defined CUR_AUTORUN (
  set "EXPECTED=doskey ds=call \"%USERPROFILE%\.dswitcher\ds.cmd\" $*"
  if /I "!CUR_AUTORUN!"=="!EXPECTED!" (
    reg delete "%REGKEY%" /v "%REGVAL%" /f >nul 2>&1
  )
)

echo Done.
endlocal
exit /b 0
