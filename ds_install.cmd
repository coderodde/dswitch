copy ds.cmd %USERPROFILE%\.dswitcher\ds.cmd
copy x64\Debug\dswitch.exe %USERPROFILE%\.dswitcher\dswitch.exe

reg add "HKCU\Software\Microsoft\Command Processor" ^
 /v AutoRun ^
 /t REG_EXPAND_SZ ^
 /d "doskey ds=call \"%USERPROFILE%\.dswitcher\ds.cmd\" $*"
