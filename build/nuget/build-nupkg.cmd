@ECHO OFF

PUSHD %~dp0

WHERE /Q nuget >NUL
IF %ERRORLEVEL% NEQ 0 ( 
    ECHO nuget not found.
    ECHO.
    ECHO Run "%~pd0download-nuget.cmd" to download the latest version, or update PATH as appropriate.
    GOTO END
)

nuget pack %~dp0Win2D.nuspec -nopackageanalysis -outputdirectory %~dp0..\..\bin %*
nuget pack %~dp0Win2D-debug.nuspec -nopackageanalysis -outputdirectory %~dp0..\..\bin %*

:END

POPD
