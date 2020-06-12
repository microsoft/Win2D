@echo off

SETLOCAL

IF "%VisualStudioVersion%" LSS "16.0" (
    GOTO WRONG_COMMAND_PROMPT
)

WHERE /Q nuget >NUL
IF %ERRORLEVEL% NEQ 0 ( 
    ECHO nuget not found.
    ECHO.
    ECHO Run "%~pd0download-nuget.cmd" to download the latest version, or update PATH as appropriate.
    GOTO END
)

@REM Restore packages manually, since this does not get picked up by Win2D.proj
set Platform=AnyCPU
set Configuration=Release
nuget.exe restore .\Microsoft.Graphics.Win2D.sln
set Platform=AnyCPU
set Configuration=Debug
nuget.exe restore .\Microsoft.Graphics.Win2D.sln


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

msbuild "%~dp0tools\docs\BuildDocs.proj" /nologo /v:m /p:IntellisenseOnly=true

IF %ERRORLEVEL% NEQ 0 (
    ECHO Build failed; aborting.
    GOTO END
)

ECHO.

CALL "%~dp0build\nuget\build-nupkg.cmd" 0-local
GOTO END


:WRONG_COMMAND_PROMPT

ECHO Please run this script from a Developer Command Prompt for VS2019
ECHO.
PAUSE
GOTO END

:END
