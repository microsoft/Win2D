@echo off

SETLOCAL

IF "%VisualStudioVersion%" LSS "15.0" (
    GOTO WRONG_COMMAND_PROMPT
)

WHERE /Q msbuild >NUL
IF %ERRORLEVEL% NEQ 0 ( 
    ECHO Error: It appears that 'msbuild' is not available in this environment. 
    ECHO.
    GOTO WRONG_COMMAND_PROMPT
)

msbuild "%~dp0Win2D.proj" /v:m /maxcpucount /nr:false /p:BuildTests=false /p:BuildTools=false /p:BuildDocs=false

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
GOTO END


:WRONG_COMMAND_PROMPT

ECHO Please run this script from a Developer Command Prompt for VS2017
ECHO.
PAUSE
GOTO END

:END
