@echo off

SETLOCAL

IF "%VisualStudioVersion%" LSS "12.0" (
    GOTO WRONG_COMMAND_PROMPT
)

SET NOUAP=

IF "%1" NEQ "" (
    IF "%1" == "nouap" (
        SET NOUAP=1
    ) ELSE (
        GOTO SHOW_USAGE
    )
)

IF NOT "%NOUAP%" == "1" (
    IF "%VisualStudioVersion%" LSS "14.0" (
        ECHO Warning: Visual Studio 2015 or higher required to build with Universal Windows Platform support.
        ECHO          Building without Universal Windows Platform support.
        ECHO.
        SET NOUAP=1
    )
)

WHERE /Q msbuild >NUL
IF %ERRORLEVEL% NEQ 0 ( 
    ECHO Error: It appears that 'msbuild' is not available in this environment. 
    ECHO.
    GOTO WRONG_COMMAND_PROMPT
)


SET BUILD_UAP=
IF "%NOUAP%" == "1" (
    SET BUILD_UAP=/p:BuildUAP=false
)

msbuild "%~dp0Win2D.proj" /v:m /maxcpucount /nr:false /p:BuildTests=false /p:BuildTools=false /p:BuildDocs=false %BUILD_UAP%

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

SET OVERRIDE_NUGET_PACKAGE=

IF "%NOUAP%" == "1" (
    SET OVERRIDE_NUGET_PACKAGE=Win2D-NoUAP
)

CALL "%~dp0build\nuget\build-nupkg.cmd" local
GOTO END


:SHOW_USAGE

ECHO %0 [nouap]
ECHO.
ECHO  nouap: pass this to disable building Universal Windows Platform support
GOTO END

:WRONG_COMMAND_PROMPT

ECHO Please run this script from the appropriate command prompt:
ECHO.
ECHO For Visual Studio 2013, building for Windows / Phone 8.1:
ECHO - Visual Studio 2013 Command Prompt
ECHO.
ECHO For Visual Studio 2015, building for Windows / Phone 8.1 and Universal Windows Platform:
ECHO - MSBuild Command Prompt for VS2015
ECHO.
PAUSE
GOTO END

:END
