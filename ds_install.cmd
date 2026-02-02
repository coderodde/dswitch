@echo off

mkdir "%USERPROFILE%\.dswitcher"
copy /Y "ds.cmd" "%USERPROFILE%\.dswitcher\ds.cmd"
copy /Y "x64\Release\dswitch.exe" "%USERPROFILE%\.dswitcher\dswitch.exe"
echo home ~>>"%USERPROFILE%\.dswitcher\tags"

reg add "HKCU\Software\Microsoft\Command Processor" ^
 /v AutoRun ^
 /t REG_EXPAND_SZ ^
 /d "doskey ds=call \"%USERPROFILE%\.dswitcher\ds.cmd\" $*" ^
 /f
