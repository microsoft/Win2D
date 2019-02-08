# XamlIslandSample

This sample shows how to use Win2D in WPF XAML Islands on current .NET Core 3 preview. Obviously this is only useful for testing and bug fixing right now. The whole process will change as .NET Core 3 / XAML Island are under active development.
The sample was developed and tested with the following components. Any other configuration may or may not work.

- Windows 10 19H1 Insider Preview 64 bit
- [Windows SDK 17763 or higher](https://go.microsoft.com/fwlink/?prd=11966&pver=1.0&plcid=0x409&clcid=0x409&ar=Flight&sar=Sdsurl&o1=17763)
- [.NET Core SDK 3.0.100-preview-010184](https://dotnet.microsoft.com/download/thank-you/dotnet-sdk-3.0.100-preview2-windows-x64-installer)
- [VS 16.0 Preview 2](https://visualstudio.microsoft.com/thank-you-downloading-visual-studio/?sku=enterprise&ch=pre&rel=16)
	*IMPORTANT*: Copy VC\Auxiliary\Build\14.16\Microsoft.VCToolsVersion.14.16.props -> VC\Auxiliary\Build\Microsoft.VCToolsVersion.default.props (https://developercommunity.visualstudio.com/content/problem/434385/vs2019-preview-2-targetframeworkversion-or-platfor.html) 

The sample consists of 2 projects

##### XamlIslandSample.Desktop (.NET Core 3 Wpf App)
One window with a CanvasControl and a CanvasSwapChainPanel. Note that the CanvasControl currently doesn't work in XAML islands (https://github.com/Microsoft/Win2D/issues/643)
- Only x64 is supported by the sample
- Set Property RuntimeIdentifiers to win-x64 in .csproj (with "s" at the end!)
- Reference assembly Windows.winmd
- Reference package System.Runtime.WindowsRuntime 4.6.0-preview.19073.11
- Reference package System.Runtime.WindowsRuntime.UI.Xaml 4.6.0-preview.19073.11
- Reference package Microsoft.Toolkit.Wpf.UI.XamlHost 6.0.0-preview1
- Reference Microsoft.Graphics.Canvas.winmd

	Adding a simple project reference to winrt.dll.uap fails with `error NETSDK1007: Cannot find project info for 'winrt\lib\winrt.lib.uap.vcxproj'. This can indicate a missing project reference.` So I added a assembly reference with hint path to Microsoft.Graphics.Canvas.winmd

##### XamlIslandSample.Package (Windows Application Package)
Loading C++ WinRT components like Win2D into managed processes only works with APPX package
- Reference project XamlIslandSample.Desktop
- Add C++ Runtime packages to Package.appxmanifest (see https://support.microsoft.com/en-us/help/3176696/c-runtime-framework-packages-for-desktop-bridge for details)

```xml
<PackageDependency Name="Microsoft.VCLibs.140.00" MinVersion="14.0.26706.0" Publisher="CN=Microsoft Corporation, O=Microsoft Corporation, L=Redmond, S=Washington, C=US" />
<PackageDependency Name="Microsoft.VCLibs.140.00.Debug" MinVersion="14.0.27305.0" Publisher="CN=Microsoft Corporation, O=Microsoft Corporation, L=Redmond, S=Washington, C=US" />
```
Both packages should be deployed automatically during sample deployment but usually can be found in C:\Program Files (x86)\Microsoft SDKs\Windows Kits\10\ExtensionSDKs\Microsoft.VCLibs\14.0\Appx

##### How to build, start and debug

To start the sample build, deploy and start XamlIslandSample.Package. At the time of writing there seems to be a bug in VS 2019 that leads to unreliable detecting changes of XamlIslandSample.Desktop. Manually rebuild and deploy of XamlIslandSample.Package helps.

To debug the Win2D components select `Mixed (Managed and Native)` under Debug / Debugger Type in XamlIslandSample.Package properties.