@echo off
set "DSDIR=%USERPROFILE%\.dswitcher"
set "DSCMD=%DSDIR%\ds_command.bat"
set "DSEXE=%DSDIR%\dswitch.exe"

set DSWITCH_SHELL=cmd
del "%DSCMD%" 2>nul
set DSWITCH_SHELL=

"%DSEXE%" %*

if exist "%DSCMD%" (
    call "%DSCMD%"
    del "%DSCMD%" 2>nul
)
