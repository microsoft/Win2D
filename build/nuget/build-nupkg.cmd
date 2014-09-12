@ECHO OFF

REM
REM Version is read from the VERSION file.
REM
REM Command-line parameter specfies a prerelease string to append to the version
REM
REM Say VERSION contains "0.0.3" then:
REM
REM build-nupkg                     <-- generates package with version 0.0.3
REM build-nupkg build-140912-1100   <-- generates package with version 0.0.3-build-140912-1100
REM

PUSHD "%~dp0"

WHERE /Q nuget >NUL
IF %ERRORLEVEL% NEQ 0 ( 
    ECHO nuget not found.
    ECHO.
    ECHO Run "%~pd0download-nuget.cmd" to download the latest version, or update PATH as appropriate.
    GOTO END
)

SET /p VERSION=<VERSION

IF NOT "%1" == "" (
    SET VERSION=%VERSION%-%1
)

nuget pack Win2D.nuspec -nopackageanalysis -outputdirectory ..\..\bin -version %VERSION%
nuget pack Win2D-debug.nuspec -nopackageanalysis -outputdirectory ..\..\bin -version %VERSION%

:END

POPD
