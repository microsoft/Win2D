# Version numbers look like this: 
# [CI]       Microsoft.Graphics.Canvas.1.0.0-CI-221220.1
# [Release]  Microsoft.Graphics.Canvas.1.0.0
# [Nightly]  Microsoft.Graphics.Canvas.1.0.0-WinAppSDK.1.0.0-20211122.1-CI-stable-Nightly.0
#
# Prereleases look like:
# [CI]       Microsoft.Graphics.Canvas.1.0.0-prerelease-CI-221220.1
# [Release]  Microsoft.Graphics.Canvas.1.0.0-prerelease
# [Nightly]  Microsoft.Graphics.Canvas.1.0.0-prerelease-WinAppSDK.1.0.0-20211122.1-CI-stable-Nightly.0
#
# Pipeline name will be just the version number (e.g. 1.27.0-CI-221220.1)

Write-Host "Setting versions for CI/Release/Nightly pipelines"
Write-Host Pipeline type: ${{ parameters.PipelineType }}
Write-Host Package Suffix: $env:packageSuffix

if ( "${{ parameters.PipelineType }}" -eq "Nightly" ) {
    # Reads the WinAppSDK Version from eng\Version.Details.xml
    # Used to read the nightly WinAppSDK version for building nightly versions of Win2D
    $file = "eng\Version.Details.xml"
    $xmlfile = [xml](Get-Content $file)

    Write-Host "Found Version.Details.xml file."
    $xmlfile.Dependencies.ProductDependencies.Dependency | Where-Object Name -eq 'Microsoft.WindowsAppSDK' | ForEach-Object {
        $WIN_APP_SDK_VERSION = $_.Version
        Write-Host "The WinAppSDK Version is $WIN_APP_SDK_VERSION"
        Write-Host "##vso[task.setvariable variable=winappsdk_version]$WIN_APP_SDK_VERSION"
        Write-Host "##vso[task.setvariable variable=winappsdk_version;isOutput=true]$WIN_APP_SDK_VERSION"
    }
    $env:packageSuffix = $env:packageSuffix + "-WinAppSDK." + $WIN_APP_SDK_VERSION
}

Write-Host Reading from VERSION file
$versionMMP = Get-Content build\nuget\VERSION
$preRelease = Get-Content build\nuget\VERSION-PRERELEASE

# If the package has a prelease version, add it to the version
if ( -not [String]::IsNullOrWhiteSpace($preRelease) ) {
    $versionMMP = $versionMMP + "-" + $preRelease
}

# If the pipeline has a suffix, add it to the name
$pipelineName = $versionMMP 
if ( -not [String]::IsNullOrWhiteSpace($env:packageSuffix) ) {
    $pipelineName = $pipelineName + "-" + $env:packageSuffix
}

Write-Host Setting pipeline name to: $pipelineName
Write-Host "##vso[build.updatebuildnumber]$pipelineName"

Write-Host Done.