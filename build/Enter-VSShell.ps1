param(
  [Parameter(Mandatory=$true)][String]$vsPath)

Write-Host "Enter-VSShell.ps1 -vsPath $vsPath"

$devShellModule = Join-Path $vsPath "Common7\Tools\Microsoft.VisualStudio.DevShell.dll"
if (!(Test-Path $devShellModule))
{
  Write-Error "Cannot find dev shell module $devShellModule"
  Dir -Path $vsPath -Rec |% { Write-Host $_.FullName }
}
else
{
  Import-Module $devShellModule
  Write-Host "Enter-VsDevShell -VsInstallPath $vsPath -SkipAutomaticLocation"
  Enter-VsDevShell -VsInstallPath $vsPath -SkipAutomaticLocation
}

