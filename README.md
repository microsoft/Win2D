# Win2D

Win2D is an easy-to-use Windows Runtime API for immediate mode 2D graphics
rendering with GPU acceleration. It is available to C# and C++ developers
writing Windows apps for Windows 8.1, Windows Phone 8.1 and Windows 10. It
utilizes the power of Direct2D, and integrates seamlessly with XAML and
CoreWindow.

##### Where to get it
- [NuGet packages](http://www.nuget.org/packages/Win2D)
- [Source code](http://github.com/Microsoft/Win2D)

##### How to use it
- [Documentation](http://microsoft.github.io/Win2D)
- Example Gallery
    [source code](https://github.com/Microsoft/Win2D/tree/master/tests/ExampleGallery/Shared),
    [Windows](http://apps.microsoft.com/windows/en-us/app/win2d-example-gallery/b668cfe1-e280-4c1e-adc1-09b7981ab084) and
    [Phone](http://www.windowsphone.com/en-us/store/app/win2d-example-gallery/8797b9cb-1443-475f-9a43-dd959de6fcc1) apps

##### More info
- [Features](http://github.com/Microsoft/Win2D/wiki/Features) - *what's implemented so far*
- [Backlog](http://github.com/Microsoft/Win2D/wiki/Backlog) - *what we plan to add next*
- [Team blog](http://blogs.msdn.com/b/win2d)
- [Report a bug or ask a question](http://github.com/Microsoft/Win2D/issues)
- [License](http://www.apache.org/licenses/LICENSE-2.0.html)
- [Contributing](http://github.com/Microsoft/Win2D/blob/master/CONTRIBUTING.md)

## Code Example
To give you a flavor of what the code looks like, here is a snippet of XAML:
```xml
xmlns:canvas="using:Microsoft.Graphics.Canvas.UI.Xaml"

<Grid>
    <canvas:CanvasControl Draw="canvasControl_Draw" ClearColor="CornflowerBlue" />
</Grid>
```
and C#:
```cs
void canvasControl_Draw(CanvasControl sender, CanvasDrawEventArgs args)
{
    args.DrawingSession.DrawEllipse(155, 115, 80, 30, Colors.Black, 3);
    args.DrawingSession.DrawText("Hello, world!", 100, 100, Colors.Yellow);
}
```

## Using Win2D

The [documentation](http://microsoft.github.io/Win2D) explains how to install Visual 
Studio, add the Win2D NuGet package to your project, and get started using the API.

## Building Win2D from source

##### Clone Repository
- Go to 'View' -> 'Team Explorer' -> 'Local Git Repositories' -> 'Clone'
- Add the Win2D repository URL (https://github.com/Microsoft/Win2D.git) and hit 'Clone'

##### Build NuGet Packages
- If using Visual Studio 2013:
  - In Windows search, type 'Visual Studio Tools', and select that folder to open it
  - Launch 'Developer Command Prompt for VS2013' 
- If using Visual Studio 2015:
  - Launch 'MSBuild Command Prompt for VS2015'
- Change directory to your cloned Win2D repository and run 'build'

##### Point Visual Studio at the resulting 'bin' directory
- In Visual Studio, go to 'Tools' -> 'NuGet Package Manager' -> 'Package Manager Settings'
- Choose 'Package Sources'
- Click the '+' button to add a new source
- Set 'Name' to 'Win2D' (or a name of your choosing)
- Set 'Source' to the full path to the 'bin' directory (inside your cloned Win2D repository)
- Click the 'Update' button
- Click 'OK'

Locally built versions of Win2D are marked as prerelease, so you must change the 'Stable 
Only' setting to 'Include Prerelease' when adding them to your project.

If you are building with Visual Studio 2013, then the generated package is
called "Win2D-NoUAP".  This contains everything the usual Win2D package contains
apart from any universal app platform binaries.
