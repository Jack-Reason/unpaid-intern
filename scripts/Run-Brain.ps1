#Requires -Version 5.1
<#
.SYNOPSIS
  Run the unpaid-intern brain against the local mod bridge (or mock).
.PARAMETER Mode
  mock | tick | loop
.PARAMETER Message
  Optional pioneer message for tick mode.
#>
param(
  [ValidateSet("mock", "tick", "loop")]
  [string]$Mode = "mock",
  [string]$Bridge = "http://127.0.0.1:8765",
  [string]$Provider = "",
  [string]$Message = "",
  [double]$Interval = 2.0
)
$ErrorActionPreference = "Stop"
$RepoRoot = Split-Path -Parent $PSScriptRoot
$BrainDir = Join-Path $RepoRoot "brain"
$VenvPython = Join-Path $BrainDir ".venv\Scripts\python.exe"
if (-not (Test-Path $VenvPython)) {
  Write-Host "No venv found — running scripts\Setup-Brain.ps1 first..."
  & (Join-Path $PSScriptRoot "Setup-Brain.ps1")
}
Set-Location $BrainDir
$argsList = @("-m", "unpaid_intern.cli", $Mode)
if ($Mode -ne "mock") {
  $argsList += @("--bridge", $Bridge)
  if ($Provider) { $argsList += @("--provider", $Provider) }
}
if ($Mode -eq "tick" -and $Message) { $argsList += @("-m", $Message) }
if ($Mode -eq "loop") { $argsList += @("--interval", "$Interval") }
& $VenvPython @argsList
