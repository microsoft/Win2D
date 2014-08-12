@echo off

IF "%VisualStudioVersion" LSS "12.0" (
    ECHO Please run this script from a Visual Studio 2013 Command Prompt.
    PAUSE
    GOTO END
)

WHERE /Q msbuild >NUL
IF %ERRORLEVEL% NEQ 0 ( 
    ECHO Error: It appears that 'msbuild' is not available in this environment. 
    ECHO Please run this script from a Visual Studio 2013 Command Prompt.
    GOTO END
)

msbuild %~dp0canvas.proj /v:m /maxcpucount /p:BuildTests=false /p:BuildTools=false /p:BuildDocs=false

IF %ERRORLEVEL% NEQ 0 (
    ECHO Build failed; aborting.
    GOTO END
)

ECHO.

CALL %~dp0build\nuget\build-nupkg.cmd

:END