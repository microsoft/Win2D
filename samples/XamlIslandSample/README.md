#XamlIslandSample

This sample shows how to use Win2D in WPF XAML islands on current .NET Core 3 preview. Obviously this is only useful for testing and bug fixing. The whole process will change as .NET Core 3 / XAML Island are under active development.
The sample was developed and tested with the following components. Any other configuration may or may not work.

- Windows 10 17763 (1809) 64 bit
- [Windows SDK 18298](https://go.microsoft.com/fwlink/?prd=11966&pver=1.0&plcid=0x409&clcid=0x409&ar=Flight&sar=Sdsurl&o1=18298)
- [.NET Core SDK 3.0.100-preview-010184](https://dotnet.microsoft.com/download/thank-you/dotnet-sdk-3.0.100-preview2-windows-x64-installer)
- [VS 16.0 Preview 2](https://visualstudio.microsoft.com/thank-you-downloading-visual-studio/?sku=enterprise&ch=pre&rel=16)

The sample consists of 2 projects

##### XamlIslandSample.Desktop (.NET Core 3 Wpf App)
- Only x64 is supported by the sample
- Set Property RuntimeIdentifiers to win-x64 in .csproj (with "s" at the end!)
- Reference Windows.winmd (from SDK 18298 or higher)
- Reference package System.Runtime.WindowsRuntime (4.6.0-preview.19073.11)
- Reference package System.Runtime.WindowsRuntime.UI.Xaml (4.6.0-preview.19073.11)
- Reference Microsoft.Toolkit.Wpf.UI.XamlHost
	There is a 6.0 preview package on https://dotnet.myget.org/gallery/uwpcommunitytoolkit that supports .NET Core 3 but unfortunately only on Windows 10 19H1 preview. Threrefore I added the 6.0.0 branch of Microsoft.Toolkit.Wpf.UI.XamlHost project to the solution and included https://github.com/windows-toolkit/Microsoft.Toolkit.Win32/pull/74 that adds support for Windows 10 1809.
- Reference Microsoft.Graphics.Canvas
	Adding a simple project reference to winrt.dll.uap fails with "error NETSDK1007: Cannot find project info for 'winrt\lib\winrt.lib.uap.vcxproj'. This can indicate a missing project reference." So I added a assembly reference with hint path to Microsoft.Graphics.Canvas.winmd

##### XamlIslandSample.Package (Windows Application Package)
- Reference XamlIslandSample.Desktop project
- Add to Package.appxmanifest (both packages should be deployed automatically but can be found in C:\Program Files (x86)\Microsoft SDKs\Windows Kits\10\ExtensionSDKs\Microsoft.VCLibs\14.0\Appx)
```xml
<PackageDependency Name="Microsoft.VCLibs.140.00" MinVersion="14.0.26706.0" Publisher="CN=Microsoft Corporation, O=Microsoft Corporation, L=Redmond, S=Washington, C=US" />
<PackageDependency Name="Microsoft.VCLibs.140.00.Debug" MinVersion="14.0.27305.0" Publisher="CN=Microsoft Corporation, O=Microsoft Corporation, L=Redmond, S=Washington, C=US" />
```



