@echo off
setlocal
pushd "%~dp0"

where /q fxc >nul
if %errorlevel% neq 0 ( 
    echo fxc not found.
    goto WRONG_COMMAND_PROMPT
)

if "%WindowsSdkDir%" == "" ( 
    goto WRONG_COMMAND_PROMPT
)

set INCLUDEPATH="%WindowsSdkDir%\Include\%WindowsSDKVersion%\um"

if not exist %INCLUDEPATH%\d2d1effecthelpers.hlsli ( 
    echo d2d1effecthelpers.hlsli not found.
    goto WRONG_COMMAND_PROMPT
)

call :COMPILE Dissolve.hlsl || goto END
call :COMPILE Ripples.hlsl  || goto END
call :COMPILE Sketch.hlsl   || goto END

goto END

:COMPILE
	echo.
	echo Compiling %1

	fxc %1 /nologo /T lib_4_0_level_9_3_ps_only /D D2D_FUNCTION /D D2D_ENTRY=main /Fl %~n1.fxlib /I %INCLUDEPATH%                        || exit /b
	fxc %1 /nologo /T ps_4_0_level_9_3 /D D2D_FULL_SHADER /D D2D_ENTRY=main /E main /setprivate %~n1.fxlib /Fo:%~n1.bin /I %INCLUDEPATH% || exit /b

	del %~n1.fxlib
	exit /b

:WRONG_COMMAND_PROMPT
echo Please run from a Developer Command Prompt for VS2015.

:END
popd
exit /b %errorlevel%
