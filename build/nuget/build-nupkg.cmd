@ECHO OFF

REM
REM Version is read from the VERSION file.
REM
REM Command-line parameter specifies optionally "signed",
REM followed by an optional prerelease string to append to the version.
REM
REM Say VERSION contains "0.0.3" then:
REM
REM build-nupkg                     <-- generates package with version 0.0.3
REM build-nupkg build-140912-1100   <-- generates package with version 0.0.3-build-140912-1100
REM
REM The "signed" flag is intended for internal Microsoft use. This generates a
REM package without a prerelease version number, using a license agreement
REM associated with Microsoft releasing pre-built binaries, and pulling the
REM binaries from a different location to usual. This script does not actually
REM perform signing: the expectation is that previous tooling has already
REM signed the files and copied them to the right place for it to pick up.
REM

SETLOCAL

PUSHD "%~dp0"

WHERE /Q nuget >NUL
IF %ERRORLEVEL% NEQ 0 ( 
    ECHO nuget not found.
    ECHO.
    ECHO Run "%~pd0download-nuget.cmd" to download the latest version, or update PATH as appropriate.
    GOTO END
)

SET /p VERSION=<VERSION

IF "%1" == "signed" (
    SHIFT
    SET BIN=bin\signed
    SET OUTDIR=..\..\bin\signed
    SET LICENSE_URL=http://www.microsoft.com/web/webpi/eula/eula_win2d_10012014.htm
    SET REQUIRE_LICENSE_ACCEPTANCE=true
) else (
    SET BIN=bin
    SET OUTDIR=..\..\bin
    SET LICENSE_URL=http://github.com/Microsoft/Win2D/blob/master/LICENSE.txt
    SET REQUIRE_LICENSE_ACCEPTANCE=false
)

IF NOT "%1" == "" (
    SET VERSION=%VERSION%-%1
)

SET NUGET_ARGS=^
    -nopackageanalysis ^
    -basepath ..\.. ^
    -outputdirectory %OUTDIR% ^
    -version %VERSION% ^
    -properties bin=%BIN%;LicenseUrl=%LICENSE_URL%;RequireLicenseAcceptance=%REQUIRE_LICENSE_ACCEPTANCE%

nuget pack Win2D.win81.nuspec %NUGET_ARGS%
IF %ERRORLEVEL% NEQ 0 GOTO END

nuget pack Win2D.uwp.nuspec %NUGET_ARGS%
IF %ERRORLEVEL% NEQ 0 GOTO END

:END

POPD
EXIT /B %ERRORLEVEL%
