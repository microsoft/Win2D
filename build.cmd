@echo off

IF "%VisualStudioVersion%" LSS "12.0" (
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

:: TODO - once ARM UAP builds work again, roll this back to a single invocation for all device types
msbuild "%~dp0Win2D.proj" /v:m /maxcpucount /p:BuildTests=false /p:BuildTools=false /p:BuildDocs=false /p:BuildUAP=false
msbuild "%~dp0Win2D.proj" /v:m /maxcpucount /p:BuildTests=false /p:BuildTools=false /p:BuildDocs=false /p:BuildWindows=false /p:BuildPhone=false /p:BuildPlatforms="Win32;x64;AnyCPU"

IF %ERRORLEVEL% NEQ 0 (
    ECHO Build failed; aborting.
    GOTO END
)

msbuild "%~dp0tools\docs\BuildDocs.proj" /nologo /v:m /p:IntellisenseOnly=true

IF %ERRORLEVEL% NEQ 0 (
    ECHO Build failed; aborting.
    GOTO END
)

ECHO.

CALL "%~dp0build\nuget\build-nupkg.cmd" local

:END