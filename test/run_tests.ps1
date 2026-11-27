# ============================================================
#  Automated tests (PowerShell / cmd).  Use test.cmd in the project root.
#
#  For each folder test\auto\<name>\
#    input.txt  : fed to build\atm.exe as standard input
#    expect.txt : every line must appear in the output
#                 (a line starting with '!' must NOT appear)
#    output.txt : the program output of this run (created here)
#
#  Optional argument: a name filter, e.g.  test.cmd 23   runs tests whose name contains "23"
# ============================================================
param([string]$Filter = "")

$ErrorActionPreference = "Stop"
$root = Split-Path -Parent $PSScriptRoot
Set-Location $root

$exe = Join-Path $root "build\atm.exe"
if (-not (Test-Path $exe)) {
    Write-Host "build\atm.exe not found. Run build.cmd first." -ForegroundColor Red
    exit 1
}

$utf8 = New-Object System.Text.UTF8Encoding($false)
$pass = 0
$fail = 0
$failed = @()

$tests = Get-ChildItem (Join-Path $root "test\auto") -Directory | Sort-Object Name
if ($Filter -ne "") { $tests = $tests | Where-Object { $_.Name -like "*$Filter*" } }

foreach ($t in $tests) {
    $inFile = Join-Path $t.FullName "input.txt"
    $expFile = Join-Path $t.FullName "expect.txt"
    $outFile = Join-Path $t.FullName "output.txt"

    Remove-Item (Join-Path $root "ATM_*_transaction_history.txt") -ErrorAction SilentlyContinue
    # Use cmd redirection (PowerShell 5.1 does not support '<' input redirection)
    cmd /d /c "`"$exe`" < `"$inFile`" > `"$outFile`" 2>&1"

    $output = [System.IO.File]::ReadAllText($outFile, $utf8)
    $ok = $true
    foreach ($raw in [System.IO.File]::ReadAllLines($expFile, $utf8)) {
        $line = $raw.TrimEnd("`r")
        if ($line -eq "") { continue }
        if ($line.StartsWith("!")) {
            $text = $line.Substring(1)
            if ($output.Contains($text)) { Write-Host "  [$($t.Name)] should NOT appear: $text" -ForegroundColor Yellow; $ok = $false }
        } else {
            if (-not $output.Contains($line)) { Write-Host "  [$($t.Name)] missing: $line" -ForegroundColor Yellow; $ok = $false }
        }
    }

    if ($ok) { Write-Host "PASS $($t.Name)" -ForegroundColor Green; $pass++ }
    else     { Write-Host "FAIL $($t.Name)" -ForegroundColor Red; $fail++; $failed += $t.Name }
}
Remove-Item (Join-Path $root "ATM_*_transaction_history.txt") -ErrorAction SilentlyContinue

Write-Host "----------------------------------------"
Write-Host "Total: $($pass + $fail), passed: $pass, failed: $fail"
if ($fail -gt 0) {
    Write-Host "Failed: $($failed -join ', ')" -ForegroundColor Red
    Write-Host "See test\auto\<name>\output.txt for the full output."
    exit 1
}
exit 0
