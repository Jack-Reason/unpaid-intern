#Requires -Version 5.1
<#
.SYNOPSIS
  Create a Python 3.11+ venv and install unpaid-intern brain (+dev) on Windows.
#>
$ErrorActionPreference = "Stop"
$RepoRoot = Split-Path -Parent $PSScriptRoot
$BrainDir = Join-Path $RepoRoot "brain"
Set-Location $BrainDir

$Py = Get-Command python -ErrorAction SilentlyContinue
if (-not $Py) { throw "python not found on PATH. Install Python 3.11+ for Windows." }

Write-Host "Using: $($Py.Source)"
& python -c "import sys; assert sys.version_info >= (3,11), sys.version"

$Venv = Join-Path $BrainDir ".venv"
if (-not (Test-Path $Venv)) {
  & python -m venv $Venv
}
$Activate = Join-Path $Venv "Scripts\Activate.ps1"
. $Activate
& python -m pip install --upgrade pip
& pip install -e ".[dev]"
Write-Host "Brain venv ready. Activate with: $Activate"
