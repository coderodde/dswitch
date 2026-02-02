@echo off
setlocal EnableExtensions EnableDelayedExpansion

set "DSDIR=%USERPROFILE%\.dswitcher"
rmdir /S /Q "%DSDIR%"
rmdir "%DSDIR%"

REM 2) Remove AutoRun value IF it matches what we installed
set "REGKEY=HKCU\Software\Microsoft\Command Processor"
set "REGVAL=AutoRun"

REM Query the current AutoRun (if it exists)
for /f "skip=2 tokens=1,2,*" %%A in ('reg query "%REGKEY%" /v "%REGVAL%" 2^>nul') do (
  set "CUR_AUTORUN=%%C"
)

if defined CUR_AUTORUN (
  REM Normalize expected string (reg stores it without surrounding quotes sometimes)
  set "EXPECTED=doskey ds=call \"%USERPROFILE%\.dswitcher\ds.cmd\" $*"

  if /I "!CUR_AUTORUN!"=="!EXPECTED!" (
    echo Removing AutoRun value...
    reg delete "%REGKEY%" /v "%REGVAL%" /f >nul 2>&1
  ) else (
    echo AutoRun exists but does not match dswitcher line; leaving it untouched.
    echo Current AutoRun: !CUR_AUTORUN!
  )
) else (
  echo No AutoRun value found; nothing to remove there.
)

echo Done.
endlocal
exit /b 0
