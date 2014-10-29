# Win2D

Win2D is an easy-to-use Windows Runtime API for immediate mode 2D graphics 
rendering with GPU acceleration. It is available to C# and C++ developers writing 
Windows Store apps for Windows 8.1 and Windows Phone 8.1. It utilizes the power of 
Direct2D, and integrates seamlessly with XAML and ICoreWindow.

Win2D is work in progress and evolving rapidly. The goal of releasing this code 
in an early, incomplete state is to solicit early developer feedback.

- [Features](http://github.com/Microsoft/Win2D/wiki/Features) - *what's implemented so far*
- [Backlog](http://github.com/Microsoft/Win2D/wiki/Backlog) - *what we plan to add next*
- [Documentation](http://microsoft.github.io/Win2D)
- [Team blog](http://blogs.msdn.com/b/win2d)
- [License](http://www.apache.org/licenses/LICENSE-2.0.html)
- [Contributing](http://github.com/Microsoft/Win2D/blob/master/CONTRIBUTING.md)

## Code Example
To give you a flavor of what the code looks like, here is a snippet of XAML:
```xml
xmlns:canvas="using:Microsoft.Graphics.Canvas"

<Grid>
    <canvas:CanvasControl x:Name="canvasControl" Draw="canvasControl_Draw" ClearColor="CornflowerBlue" />
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

## Setup
#### Visual Studio
- Download [here](http://www.visualstudio.com/en-us/downloads/download-visual-studio-vs.aspx)
- Requires Visual Studio 2013 Update 2 or greater
- If using Visual Studio Express, requires the 'for Windows' version (not 'for Windows Desktop')

#### Clone Repository
- Go to 'View' -> 'Team Explorer' -> 'Local Git Repositories' -> 'Clone'
- Add the Win2D repository URL (https://github.com/Microsoft/Win2D.git) and hit 'Clone'

#### Build NuGet Packages
- In Windows search, type 'Visual Studio Tools', and select that folder to open it
- Launch 'Developer Command Prompt for VS2013'
- Change directory to your cloned Win2D repository and run 'build'
- Point Visual Studio at the resulting 'bin' directory:
  - In Visual Studio, go to 'Tools' -> 'NuGet Package Manager' -> 'Package Manager Settings'
  - Choose 'Package Sources'
  - Click the '+' button to add a new source
  - Set 'Name' to 'Win2D' (or a name of your choosing)
  - Set 'Source' to the full path to the 'bin' directory (inside your cloned Win2D repository)
  - Click the 'Update' button
  - Click 'OK'

#### Quickstart
Follow the steps below to get started on writing your own apps.
 
- Go to 'File' -> 'New' -> 'Project...'
- Select 'Visual C#' -> 'Store Apps' -> 'Universal Apps'
- Create a 'Blank App (Universal Apps)'
- Set a 'Name' of your choosing
- Click 'OK'
- Go to 'Tools' -> 'NuGet Package Manager' -> 'Manage NuGet Packages for Solution...'
- If installing the locally built version, change 'Stable Only' to 'Include Prerelease'
- Select 'Win2D' package and click 'Install'
- Click 'Close'
- Change Solution platform from 'Any CPU' to x86
- You are now ready to hit F5 (Build)

A working sample can be found in the Samples directory.