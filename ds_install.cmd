@echo off

mkdir "%USERPROFILE%\.dswitcher"
copy /Y "ds.cmd" "%USERPROFILE%\.dswitcher\ds.cmd"
copy /Y "x64\Release\dswitch.exe" "%USERPROFILE%\.dswitcher\dswitch.exe"
del "%USERPROFILE%\.dswitcher\tags"
echo home C:\Users\rodio>>"%USERPROFILE%\.dswitcher\tags"

reg add "HKCU\Software\Microsoft\Command Processor" ^
 /v AutoRun ^
 /t REG_EXPAND_SZ ^
 /d "doskey ds=call \"%USERPROFILE%\.dswitcher\ds.cmd\" $*" ^
 /f
