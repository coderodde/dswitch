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

# --- Add ds function to profile (idempotent + safe) ---
$markerStart = "# >>> dswitcher >>>"
$markerEnd   = "# <<< dswitcher <<<"

$snippet = @"
$markerStart
function ds {
    & `"$dsPs1`" @args
}
$markerEnd
"@

$profileText = ""
try {
    $profileText = Get-Content -LiteralPath $PROFILE -Raw -ErrorAction Stop
} catch {
    $profileText = ""
}

if ($profileText -notmatch [regex]::Escape($markerStart)) {
    Add-Content -LiteralPath $PROFILE -Value ("`n" + $snippet + "`n")
}

# --- Activate immediately in current session ---
function ds {
    & $dsPs1 @args
}

Write-Host "Installed to $dsDir"
Write-Host "ds is available now in this session."
Write-Host "It will also be available in new PowerShell sessions via: $PROFILE"
Write-Host "Tip: if a new session doesn't pick it up, run: . `$PROFILE"
