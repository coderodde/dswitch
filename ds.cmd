@echo off
set "DSDIR=%USERPROFILE%\.dswitcher"
set "DSCMD=%DSDIR%\ds_command.bat"
set "DSEXE=%DSDIR%\dswitch.exe"

del "%DSCMD%" 2>nul

"%DSEXE%" %*

if exist "%DSCMD%" (
    call "%DSCMD%"
    del "%DSCMD%" 2>nul
)
