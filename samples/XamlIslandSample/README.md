# XamlIslandSample

This sample shows how to use Win2D in WPF XAML Islands on current .NET Core 3 preview. Obviously this is only useful for testing and bug fixing right now. The whole process will change as .NET Core 3 / XAML Island are under active development.
The sample was developed and tested with the following components. Any other configuration may or may not work.

- Windows 10 19H1 Insider Preview 64 bit
- [Windows SDK 18342 or higher](https://go.microsoft.com/fwlink/?prd=11966&pver=1.0&plcid=0x409&clcid=0x409&ar=Flight&sar=Sdsurl&o1=18342)
- [.NET Core SDK 3.0.100-preview3-010431](https://dotnet.microsoft.com/download/thank-you/dotnet-sdk-3.0.100-preview3-windows-x64-installer)
- [VS 16.0 Preview 4](https://visualstudio.microsoft.com/thank-you-downloading-visual-studio/?sku=enterprise&ch=pre&rel=16)

The sample consists of 2 projects

##### XamlIslandSample.Desktop (.NET Core 3 Wpf App)
One window with the particle example.

##### XamlIslandSample.Package (Windows Application Package)
Loading C++ WinRT components like Win2D into managed processes only works with an APPX / MSIX package

##### How to build, start and debug

To start the sample from Visual Studio build, deploy and start XamlIslandSample.Package. At the time of writing there seems to be a bug in VS 2019 that leads to unreliable detecting changes of XamlIslandSample.Desktop. Manually rebuild and deploy of XamlIslandSample.Package helps.

To debug the Win2D components select `Mixed (Managed and Native)` under Debug / Debugger Type in XamlIslandSample.Package properties.

# Use Win2D NuGet package in WPF XAML islands (.NET Core 3 / .NET Framework)

The current preview package has some support for using Win2D in XAML Islands but there are some manual steps to be done
1. At the moment the NuGet package is not published so you have to build a local Win2D package from this branch. See https://github.com/Microsoft/Win2D#building-win2d-from-source for details.
2. Create a WPF app (.NET Core or .NET Framework) that references the Win2D NuGet package along with a Windows Application Packaging Project. An appx / msix package is necessary to run any third party WinRT Components like Win2D in a desktop app. This package can be sideloaded or installed from the store.
3. You have to **tweak both project files** manually (details below). This will probably change in the future.
   - The WPF app needs to get built self-contained (everything included in the appx) and therefore needs to know which runtime should be included.
   - The appx must reference the VC Runtime. There is no way for this reference to flow from the Win2D NuGet package via the app to the appx project automatically.
   - WORKAROUND (.NET Core): The project .deps.json file is needed at runtime by a .NET Core app but gets not deployed.
   - WORKAROUND (.NET Core): At startup the .NET Core app looks up all references in another directory as WinMD files are deployed.
4. Add code that targets Win2D components. Maybe you want to reference `Microsoft.Toolkit.Wpf.UI.XamlHost` and add some XAML UI (like in https://github.com/Microsoft/Win2D/tree/xaml_islands/samples/XamlIslandSample) or you create a XAML island by your own. To smoke test the deployment you can add
    ```csharp
    Windows.UI.Xaml.Hosting.WindowsXamlManager.InitializeForCurrentThread();
    new Microsoft.Graphics.Canvas.UI.Xaml.CanvasControl();
    ```
    This is nothing visible but will load all Win2D components at runtime and thus will crash if something is not found or cannot be loaded.
5. Select `x64` or `x86` as platform (`Any CPU` won't work, `ARM` is not yet supported by XAML islands). Build, deploy and start the Packaging project.

**Steps to create the solution**

*NOTE*: This describes the steps to create a **.NET Core WPF** app. They apply to **.NET Framework WPF** apps accordingly. Two of the workarounds (".deps.json" and "duplicate in project dir") are NOT necessary in .NET Framework WPF.
- Create a new directory and open a command line
- `dotnet new sln`
- `dotnet new nugetconfig`
- Add this to `nuget.config`
    ```xml
    <add key="local" value="{path to Win2D bin directory}" />`
    ```
    *NOTE*: Alternative you can add the local source to NuGet Package Manager Settings in VS
- `dotnet new wpf -n wpfapp`
- Add this to `wpfapp\wpfapp.csproj`
  ```xml
  <PropertyGroup>
    <RuntimeIdentifiers>win-x86;win-x64</RuntimeIdentifiers>
    <Platforms>x86;x64</Platforms>
  </PropertyGroup>
  <ItemGroup>
    <Reference Include="Windows">
      <HintPath>$(MSBuildProgramFiles32)\Windows Kits\10\UnionMetadata\10.0.17763.0\Windows.winmd</HintPath>
      <IsWinMDFile>true</IsWinMDFile>
      <Private>False</Private>
    </Reference>
    <PackageReference Include="Win2D.uwp" Version="1.24.0-local" />
  </ItemGroup>

  <!-- WOKAROUND: .deps.json gets not published 
                  https://developercommunity.visualstudio.com/content/problem/462646/referencing-native-winrt-component-in-net-core-wpf.html -->
  <PropertyGroup>
    <DesktopBridgePublishItemsOutputGroupDependsOn>
      $(DesktopBridgePublishItemsOutputGroupDependsOn);
      __IncludeProjectDepsFile
    </DesktopBridgePublishItemsOutputGroupDependsOn>
  </PropertyGroup>
  <Target Name="__IncludeProjectDepsFile">
    <ItemGroup>
      <ResolvedFileToPublish Include="$(ProjectDepsFilePath)">
        <RelativePath>$(ProjectDepsFileName)</RelativePath>
      </ResolvedFileToPublish>
    </ItemGroup>
  </Target>
	```
- `dotnet sln add wpfapp`
- Open the solution in VS 2019 Preview
- Add a new Windows Application Packaging Project `wpfapp.package` with Target version `Windows 10.0.17763.0` or higher  
  *NOTE*: If you want to generate a `.msix` package (instead of `.appx`) choose `Windows 10.0.17763.0` or higher also as Target min version
- Add a project reference `wpfapp.package` -> `wpfapp`
- Add this to `wpfapp.package\wpfapp.package.wapproj`
  ```xml
  <ItemGroup>
    <VCLibTargets Include="Microsoft.VCLibs, Version=14.0">
      <PlatformTarget>$(PlatformTarget)</PlatformTarget>
    </VCLibTargets>
  </ItemGroup>

  <!-- WORKAROUND: At startup Microsoft.Graphics.Canvas.winmd is expected in the same directory as the .exe 
                   (sideloaded: AppX\XamlIslandSample.Desktop). Duplicate the .winmd item 
                   https://developercommunity.visualstudio.com/content/problem/462646/referencing-native-winrt-component-in-net-core-wpf.html -->
  <Target Name="__CopyWinMDToProjectDir" BeforeTargets="_CopyPackageFiles">
    <ItemGroup>
      <DuplicatedWinMDFiles Condition="'%(WapProjPackageFile.Extension)' == '.winmd' and '%(WapProjPackageFile.DependencyKind)' == 'Direct'" Include="%(WapProjPackageFile.Identity)">
        <CopyToTargetPath>$([System.IO.Path]::Combine($(TargetDir), %(WapProjPackageFile.SourceProject), %(WapProjPackageFile.TargetPath)))</CopyToTargetPath>
        <TargetPath>$([System.IO.Path]::Combine(%(WapProjPackageFile.SourceProject), %(WapProjPackageFile.TargetPath)))</TargetPath>
      </DuplicatedWinMDFiles>
      <AppxPackagePayload Include="@(DuplicatedWinMDFiles)" />
      <File Include="@(DuplicatedWinMDFiles)" />
      <DuplicatedUploadWinMDFiles Condition="'%(UploadWapProjPackageFile.Extension)' == '.winmd' and '%(UploadWapProjPackageFile.DependencyKind)' == 'Direct'" Include="%(UploadWapProjPackageFile.Identity)">
        <CopyToTargetPath>$([System.IO.Path]::Combine($(TargetDir), %(UploadWapProjPackageFile.SourceProject), %(UploadWapProjPackageFile.TargetPath)))</CopyToTargetPath>
        <TargetPath>$([System.IO.Path]::Combine(%(UploadWapProjPackageFile.SourceProject), %(UploadWapProjPackageFile.TargetPath)))</TargetPath>
      </DuplicatedUploadWinMDFiles>
      <AppxUploadPackagePayload Include="@(DuplicatedUploadWinMDFiles)" />
      <UploadFile Include="@(DuplicatedUploadWinMDFiles)" />
    </ItemGroup>
  </Target>
  ```


