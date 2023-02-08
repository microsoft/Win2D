# Writes the WinAppSDK version assigned to the environment variable winappsdk_version to all the nuget.config files
# Used to programatically update the nuget.config files when building nightly versions of Win2D

Write-Host "Updating packages.config files with new WinAppSDK version"
$fileList = 'winrt\lib\packages.config','winrt\dll\packages.config','winrt\test.external\packages.config','winrt\test.internal\packages.config','build\pkges\packages.config'

foreach ( $configFile in $fileList )
{
    $configPath = "$env:Build_SourcesDirectory\$configFile"
    $configXml = [xml](Get-Content $configPath)

    $configXml.packages.package | Where-Object id -eq 'Microsoft.WindowsAppSDK' | ForEach-Object {
        $_.SetAttribute("version", $env:NIGHTLY_WINAPPSDK_VERSION);
        
        Write-Host "Setting the WinAppSDK version for $configfile to $env:NIGHTLY_WINAPPSDK_VERSION"
    }

    $configXml.Save($configPath)
}

Write-Host "Done."