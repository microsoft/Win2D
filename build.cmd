@echo off

SETLOCAL

IF "%VisualStudioVersion%" LSS "16.0" (
    GOTO WRONG_COMMAND_PROMPT
)

WHERE /Q msbuild >NUL
IF %ERRORLEVEL% NEQ 0 ( 
    ECHO Error: It appears that 'msbuild' is not available in this environment. 
    ECHO.
    GOTO WRONG_COMMAND_PROMPT
)

msbuild "%~dp0Win2D.proj" /v:m /maxcpucount /nr:false /p:BuildTests=false /p:BuildTools=false /p:BuildDocs=false /bl

IF %ERRORLEVEL% NEQ 0 (
    ECHO Build failed; aborting.
    GOTO END
)

ECHO.

SET /p VERSION=<build/nuget/VERSION

CALL "%~dp0build\nuget\build-nupkg.cmd" %VERSION%-local
GOTO END


:WRONG_COMMAND_PROMPT

ECHO Please run this script from a Developer Command Prompt for VS2019
ECHO.
PAUSE
GOTO END

:END
