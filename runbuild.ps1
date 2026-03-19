# Script to run a build of Win2d - this mimics build.cmd but adds more customization options
# One of the goals is to closely mimic what the ADO pipeline does for more consistency.
# Examples:
#       Build Win2D (just Microsoft.Graphics.Canvas.dll) for all versions / revisions
#       .\runbuild.ps1
#
#       Quickly build Win2D for inner loop
#       .\runbuild.ps1 -BuildConfigurations "Debug" -BuildPlatforms "x64"
#
#       Clean (delete bin/obj files)
#       .\runbuild.ps1 -Clean
#
#       Build and run tests
#       .\runbuild.ps1 -BuildConfigurations "Debug" -BuildPlatforms "Win32;x86" -RunTests
[CmdletBinding()]
param (
    [string]$BuildConfigurations,
    [string]$BuildPlatforms,
    [switch]$BuildTests = $false,
    [switch]$BuildDocs = $false,
    [switch]$RunTests = $false,
    [switch]$Clean
)

$ErrorActionPreference = 'Stop';

if ($Clean) {
    $objDir = Join-Path $PSScriptRoot "obj"
    $binDir = Join-Path $PSScriptRoot "bin"
    if (Test-Path $objDir) { Remove-Item $objDir -Recurse }
    if (Test-Path $binDir) { Remove-Item $binDir -Recurse }
    Write-Host "Successfully cleaned binaries!" -ForegroundColor Green
    return;
}

# TODO: figure out dynamically what version of visual studio is installed
$env:VS160COMNTOOLS = "C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\Common7\Tools\"

$msbuildExe = "C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\MSBuild\Current\Bin\MSBuild.exe"
$projFile = Join-Path $PSScriptRoot "Win2D.proj"
$slnPath = Join-Path $PSScriptRoot "Microsoft.Graphics.Win2D.sln"
$nugetExe = Join-Path $PSScriptRoot "build\nuget\nuget.exe"

if (!(Test-Path $msbuildExe)) {
    Write-Error "Could not find msbuild.exe. Is Visual Studio 2019 installed?`n    Checked for msbuild.exe here: $msbuildExe"
    return;
}


if (!(Test-Path $nugetExe)) {
    Write-Error "Could not find nuget.exe ($nugetExe)`n    Please run 'build\nuget\download-nuget.cmd' to install nuget."
    return;
}


# Step 1 is nuget restore for AnyCPU projects, which MSBuild doesn't do for us

Write-Host "Beginning nuget restore..." -ForegroundColor Green

$env:Platform = "AnyCPU"
$env:Configuration = "Release"
Write-Verbose "Restoring Release, AnyCPU"
& $nugetExe restore $slnPath
$env:Configuration = "Debug"
Write-Verbose "Restoring Debug, AnyCPU"
& $nugetExe restore $slnPath

Write-Host "Beginning build..." -ForegroundColor Green

$buildConfigArg = ""
if ($BuildConfigurations) {
    $buildConfigArg = "/p:BuildConfigurations=$BuildConfigurations"
}

$buildPlatformsArg = ""
if ($BuildPlatforms) {
    $buildPlatformsArg = "/p:BuildPlatforms=$BuildPlatforms"
}

if ($RunTests) {
    $BuildTests = $true
}

Write-Output "$msbuildExe $projFile $buildConfigArg $buildPlatformsArg /p:BuildTests=$BuildTests /p:BuildDocs=$BuildDocs /p:RunTests=$RunTests"
& $msbuildExe $projFile $buildConfigArg $buildPlatformsArg "/p:BuildTests=$BuildTests" "/p:BuildDocs=$BuildDocs" "/p:RunTests=$RunTests"