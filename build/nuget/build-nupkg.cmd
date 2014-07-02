@ECHO OFF

IF "%1" == "/?" (
    ECHO Usage: %0 [/n]
    ECHO.
    ECHO Generates the Windows Canvas NuGet package.
    ECHO.
    ECHO     /n: Don't rebuild code
    ECHO.
    GOTO END
)

WHERE /Q nuget >NUL
IF NOT EXIST nuget.exe (
    ECHO nuget not found.
    ECHO.
    ECHO Run "%~pd0download-nuget.cmd" to download the latest version, or update PATH as appropriate.
    GOTO END
)


IF NOT "%1" == "/n" (
    WHERE /Q msbuild >NUL
    IF %ERRORLEVEL% NEQ 0 ( 
        ECHO Error: It appears that 'msbuild' is not available in this environment. 
        ECHO Please run this script from a Visual Studio command prompt.
        GOTO END
    )
    msbuild build.proj /v:m

    IF %ERRORLEVEL% NEQ 0 (
        ECHO Build failed; aborting.  Use /n to skip building.
        GOTO END
    )

    ECHO.
    ECHO.
    ECHO.
)

nuget pack WindowsCanvas.nuspec -nopackageanalysis -outputdirectory %~p0..\..\bin
nuget pack WindowsCanvas-debug.nuspec -nopackageanalysis -outputdirectory %~p0..\..\bin

:END