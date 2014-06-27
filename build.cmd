@echo off
IF "%VisualStudioVersion" LSS "12.0" (
    ECHO Please run this script from a Visual Studio 2013 Command Prompt
    PAUSE
    GOTO END
)

PUSHD %~dp0build\nuget
CALL build-nupkg.cmd
POPD

:END