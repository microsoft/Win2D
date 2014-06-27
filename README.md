# Windows Canvas

todo

# Using the NuGet package

Windows Canvas can be build to a nuget package that can be simply installed to a
solution or project.

## Generating a NuGet package

If nuget.exe has not already been downloaded then the latest version is always
available from http://nuget.org/nuget.exe.  The `build\nuget\download-nuget.cmd`
script downloads this to the correct location under `build\nuget`.

From a Visual Studio 2013 command prompt run:

```batchfile
build
```

This will build the required binaries and output a package to the 'bin' directory.


# Installing the NuGet package

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

* `Tools` -> `NuGet Package Manager` -> Manage NuGet Packages For Solution`
** *or* right-click on your solution/project and select `Manage NuGet Packages`
* Choose `Online` / `Windows Canvas` (or whatever name you chose)
* Click the `Install` button next to `Windows Canvas`
