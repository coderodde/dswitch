@echo off

mkdir %USERPROFILE%\.dswitcher
copy /Y ds.cmd %USERPROFILE%\.dswitcher\ds.cmd
copy /Y x64\Debug\dswitch.exe %USERPROFILE%\.dswitcher\dswitcher.exe

reg add "HKCU\Software\Microsoft\Command Processor" ^
 /v AutoRun ^
 /t REG_EXPAND_SZ ^
 /d "doskey ds=call \"%USERPROFILE%\.dswitcher\ds.cmd\" $*" ^
 /f
