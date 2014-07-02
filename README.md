# Windows Canvas

# Using the NuGet package

Windows Canvas can be build to a nuget package that can be simply installed to a
solution or project.

Two packages are available:

* _Windows Canvas_ - this package is intended for general development and
  distribution.

* _Windows Canvas (debug)_ - this package provides debug binaries for Windows
  Canvas.  This only really useful if you are debugging an issue with Windows
  Canvas and is not intended for general development.


## Generating a NuGet package

If nuget.exe has not already been downloaded then the latest version is always
available from http://nuget.org/nuget.exe.  The `build\nuget\download-nuget.cmd`
script downloads this to the correct location under `build\nuget`.

From a Visual Studio 2013 command prompt run:

```
build
```

This will build the required binaries and output packages to the 'bin' directory.


## Installing the NuGet package


### Adding NuGet to Visual Studio

If you don't have a `NuGet Package Manager` option in your `Tools` menu then you
can add NuGet to Visual Studio by:

* `Tools` -> `Extensions and Updates` -> `Online` -> `Visual Studio Gallery`
* Download and install NuGet Package Manager


### Point NuGet at your bin directory

In Visual Studio:

* `Tools` -> `NuGet Package Manager` -> `Package Manager Settings`
* Choose `Package Sources`
* Click the `+` button to add a new source
* Set `Name` to `Windows Canvas` (or a name of your choosing)
* Set `Source` to the full path to the `bin` directory
* Click the `Update` button
* Click `OK`


### Installing the package to your solution

In Visual Studio, once you have your solution open:

* `Tools` -> `NuGet Package Manager` -> `Manage NuGet Packages For Solution`
   _or_ right-click on your solution/project and select `Manage NuGet Packages`

* Choose `Online` / `Windows Canvas` (or whatever name you chose) Click the
  `Install` button next to `Windows Canvas`
