$ErrorActionPreference = "Stop"

function TryRemoveDirTree([string]$path) {
    Write-Host "Target dir: $path"
    if (-not (Test-Path -LiteralPath $path)) {
        Write-Host "  -> Directory does not exist. Skipping."
        return
    }

    # Avoid being inside it
    try { Set-Location -Path $env:USERPROFILE } catch {}

    try {
        Remove-Item -LiteralPath $path -Recurse -Force
        Write-Host "  -> Removed directory tree."
    } catch {
        Write-Host "  -> FAILED to remove directory tree: $($_.Exception.Message)"
        Write-Host "  -> Contents left:"
        Get-ChildItem -LiteralPath $path -Force | Format-Table -AutoSize
        throw
    }
}

function RemoveDswitcherBlockFromProfile([string]$profilePath) {
    if ([string]::IsNullOrWhiteSpace($profilePath)) { return }

    Write-Host "Profile: $profilePath"
    if (-not (Test-Path -LiteralPath $profilePath)) {
        Write-Host "  -> Profile file does not exist. Skipping."
        return
    }

    $markerStart = "# >>> dswitcher >>>"
    $markerEnd   = "# <<< dswitcher <<<"

    $text = Get-Content -LiteralPath $profilePath -Raw
    if ($null -eq $text) { $text = "" }

    $pattern = [regex]::Escape($markerStart) + ".*?" + [regex]::Escape($markerEnd) + "\s*"

    # Correct way: build a regex with Singleline so '.' matches newlines
    $rx = [regex]::new($pattern, [System.Text.RegularExpressions.RegexOptions]::Singleline)
    $newText = $rx.Replace($text, "")

    if ($newText -ne $text) {
        Set-Content -LiteralPath $profilePath -Value $newText -Encoding UTF8
        Write-Host "  -> Removed dswitcher block."
    } else {
        Write-Host "  -> No dswitcher block found."
    }
}


# --- 1) Remove ~/.dswitcher ---
$dsDir = Join-Path $env:USERPROFILE ".dswitcher"
TryRemoveDirTree $dsDir

# --- 2) Remove dswitcher snippet from all typical profile locations ---
Write-Host "`nRemoving dswitcher block from PowerShell profiles..."

$profilesToCheck = @()

# $PROFILE itself (current host)
$profilesToCheck += $PROFILE

# Try all standard variants (works in Windows PowerShell + PS7)
try { $profilesToCheck += $PROFILE.CurrentUserCurrentHost } catch {}
try { $profilesToCheck += $PROFILE.CurrentUserAllHosts } catch {}
try { $profilesToCheck += $PROFILE.AllUsersCurrentHost } catch {}
try { $profilesToCheck += $PROFILE.AllUsersAllHosts } catch {}

# De-dup and process
$profilesToCheck = $profilesToCheck | Where-Object { $_ } | Select-Object -Unique
foreach ($p in $profilesToCheck) {
    RemoveDswitcherBlockFromProfile $p
}

# Also remove from current session (optional nicety)
if (Get-Command ds -ErrorAction SilentlyContinue) {
    Remove-Item Function:\ds -ErrorAction SilentlyContinue
    Write-Host "`nRemoved 'ds' function from current session."
}

# --- 3) Remove CMD AutoRun value (HKCU) ---
Write-Host "`nRemoving CMD AutoRun registry value (HKCU)..."
$regPath = "HKCU:\Software\Microsoft\Command Processor"
try {
    Remove-ItemProperty -Path $regPath -Name "AutoRun" -Force
    Write-Host "  -> Removed AutoRun."
} catch {
    Write-Host "  -> AutoRun not present (or already removed)."
}

Write-Host "`nDone."
exit 0
