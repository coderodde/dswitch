# --- Paths ---
$dsDir = Join-Path $env:USERPROFILE ".dswitcher"
$dsPs1 = Join-Path $dsDir "ds.ps1"
$dsExe = Join-Path $dsDir "dswitch.exe"
$tags  = Join-Path $dsDir "tags"
$prev  = Join-Path $dsDir "prev"

# --- Create ~/.dswitcher ---
New-Item -ItemType Directory -Path $dsDir -Force | Out-Null

# --- Copy payload files ---
Copy-Item -Path "ds.ps1" -Destination $dsPs1 -Force
Copy-Item -Path (Join-Path "x64\Release" "dswitch.exe") -Destination $dsExe -Force

# --- Initialize state files ---
"home ~" | Set-Content -LiteralPath $tags -Encoding ASCII
"~"      | Set-Content -LiteralPath $prev -Encoding ASCII

# --- Ensure PowerShell profile exists ---
$profileDir = Split-Path -Parent $PROFILE
New-Item -ItemType Directory -Path $profileDir -Force | Out-Null
if (-not (Test-Path -LiteralPath $PROFILE)) {
    New-Item -ItemType File -Path $PROFILE -Force | Out-Null
}

# --- Add ds function to $PROFILE (idempotent) ---
$markerStart = "# >>> dswitcher >>>"

$profileText = ""
try {
    $profileText = Get-Content -LiteralPath $PROFILE -Raw -ErrorAction Stop
} catch {
    $profileText = ""
}

if ($profileText -notmatch [regex]::Escape($markerStart)) {
    Add-Content -Path $PROFILE -Value @"

# >>> dswitcher >>>
function ds {
    & "`$env:USERPROFILE\.dswitcher\ds.ps1" @args
}
# <<< dswitcher <<<

"@
}

# --- Activate immediately in current session ---
function ds {
    & "$env:USERPROFILE\.dswitcher\ds.ps1" @args
}

$regPath = "HKCU:\Software\Microsoft\Command Processor"
$valueName = "AutoRun"
$valueData = 'doskey ds=call "%USERPROFILE%\.dswitcher\ds.cmd" $*'

# Ensure the key exists
New-Item -Path $regPath -Force | Out-Null

# Create or overwrite the AutoRun value as REG_EXPAND_SZ
New-ItemProperty `
    -Path $regPath `
    -Name $valueName `
    -PropertyType ExpandString `
    -Value $valueData `
    -Force | Out-Null


Write-Host "Installed to $dsDir"
Write-Host "ds is available now in this session."
Write-Host "It will also be available in new PowerShell sessions via: $PROFILE"
Write-Host "Tip: to enable it in THIS session without restarting, run: . `$PROFILE"
