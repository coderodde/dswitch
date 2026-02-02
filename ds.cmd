@echo off
del ds_command.bat
"dswitch.exe" %*
if exist ds_command.bat (
    call ds_command.bat
    del ds_command.bat
)