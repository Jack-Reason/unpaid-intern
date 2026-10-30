#Requires -Version 5.1
$ErrorActionPreference = "Stop"
$RepoRoot = Split-Path -Parent $PSScriptRoot
$BrainDir = Join-Path $RepoRoot "brain"
$VenvPython = Join-Path $BrainDir ".venv\Scripts\python.exe"
if (-not (Test-Path $VenvPython)) {
  & (Join-Path $PSScriptRoot "Setup-Brain.ps1")
}
Set-Location $BrainDir
& $VenvPython -m ruff check .
& $VenvPython -m pytest -q
