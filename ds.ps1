$DSDIR = Join-Path $env:USERPROFILE ".dswitcher"
$DSCMD = Join-Path $DSDIR "ds_command.ps1"   # IMPORTANT: use .ps1 if you want to change PS directory
$DSEXE = Join-Path $DSDIR "dswitch.exe"

# Tell dswitch.exe which shell invoked it
$env:DSWITCH_SHELL = "powershell"

# Remove old command file
Remove-Item -LiteralPath $DSCMD -Force -ErrorAction SilentlyContinue

# Run dswitch.exe with all arguments
& $DSEXE @args

# Clear environment variable (optional cleanup)
Remove-Item Env:DSWITCH_SHELL -ErrorAction SilentlyContinue

# If command script was generated, dot-source it so it affects current session
if (Test-Path -LiteralPath $DSCMD) {
    . $DSCMD
    Remove-Item -LiteralPath $DSCMD -Force -ErrorAction SilentlyContinue
}
