$dsDir = Join-Path $env:USERPROFILE ".dswitcher"

# Make sure we are not inside the directory we want to delete
try {
    Set-Location -Path $env:USERPROFILE -ErrorAction Stop
} catch {
    # Ignore; deletion attempt below will still run
}

# Remove folder tree
if (Test-Path -LiteralPath $dsDir) {
    try {
        Remove-Item -LiteralPath $dsDir -Recurse -Force -ErrorAction SilentlyContinue
    } catch {
        # Diagnostics below if it still exists
    }
}

# If still exists, show why (optional diagnostics)
if (Test-Path -LiteralPath $dsDir) {
    Write-Host "Failed to remove `"$dsDir`"."
    Write-Host "Possible reasons: file is in use, permissions, or current directory."
    Write-Host "Contents left:"
    Get-ChildItem -LiteralPath $dsDir -Force | Format-Table -AutoSize
}

# Remove PowerShell profile snippet (Fix 2 installer block)
try {
    $markerStart = "# >>> dswitcher >>>"
    $markerEnd   = "# <<< dswitcher <<<"

    if (Test-Path -LiteralPath $PROFILE) {
        $text = Get-Content -LiteralPath $PROFILE -Raw

        # Remove everything from markerStart to markerEnd (inclusive), plus surrounding whitespace/newlines
        $pattern = [regex]::Escape($markerStart) + ".*?" + [regex]::Escape($markerEnd) + "\s*"
        $newText = [regex]::Replace($text, $pattern, "", "Singleline")

        if ($newText -ne $text) {
            Set-Content -LiteralPath $PROFILE -Value $newText -Encoding UTF8
        }
    }
} catch {
    # Ignore profile cleanup errors
}

# Optional: if you *also* had the CMD AutoRun set earlier, remove it (harmless if missing)
$regPath = "HKCU:\Software\Microsoft\Command Processor"
Remove-ItemProperty -Path $regPath -Name "AutoRun" -Force -ErrorAction SilentlyContinue

Write-Host "Done."
exit 0
