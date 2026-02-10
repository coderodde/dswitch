$DSDIR = Join-Path $env:USERPROFILE ".dswitcher"
$DSEXE = Join-Path $DSDIR "dswitch.exe"

# Run dswitch.exe and capture output
$target = & $DSEXE @args

# If dswitch.exe printed a path, cd into it
if ($LASTEXITCODE -eq 0 -and $target) {
    Set-Location -LiteralPath $target
}
