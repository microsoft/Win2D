@ECHO OFF
@SETLOCAL
@SETLOCAL ENABLEDELAYEDEXPANSION

:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::
:: Description:
::     This script is a convenience for building all the 
::     flavors of codegen and the DXRT solution (all 
::     platforms and configurations), all in one step. Runs 
::     the tests as well.
::
:: Usage:
::     BuildAndTestAllConfigs.cmd [path]
::
::     where [path] is the absolute path to your 
::     local Git repository. This parameter is optional.
::
::     If [path] is not specified, the script will infer
::     your Git repository path from its location (works
::     so long as it hasn't been copied someplace else).
::
::
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

WHERE /Q msbuild >NUL
IF %ERRORLEVEL% NEQ 0 ( 
    ECHO Error: It appears that 'msbuild' is not available in this environment. 
    ECHO Please run this script from a Visual Studio command prompt, or 
    ECHO ensure the PATH is configured correctly.
    GOTO END
)

IF "%1"=="/?" (
    ECHO Usage: %0 [path]
    ECHO    where [path] is the path to your local Git repository.
    ECHO    Example: BuildAndTestAllConfigs.cmd C:\Src
    GOTO END
)

SET ENABLE_PARALLEL=/m

:: Full Rebuild is necessary to work around bug #965 (task #1063). A race condition
:: can occur where IDL dependencies are not rebuilt for the components that 
:: consume them. We can change this to a build (remove this flag altogether) 
:: when the bug is fixed.
:: This bug does not occur every time, but this should prevent it.
SET USE_REBUILD=/t:Rebuild

SET BUILD_FLAGS=%ENABLE_PARALLEL% %USE_REBUILD%

SET GIT_TREE_LOCATION=%~dp0..\

IF NOT "%1"=="" (
   SET GIT_TREE_LOCATION=%1
)

set BIN_LOCATION=%GIT_TREE_LOCATION%\bin\

:: Codegen.sln
:: The codegen solution is intended to build on one platform, Any CPU. 
for %%C in (Debug Release) DO (
    msbuild %GIT_TREE_LOCATION%\Codegen.sln %BUILD_FLAGS% /p:Configuration="%%C" /p:Platform="Any CPU"
    IF !ERRORLEVEL! NEQ 0 ( 
        ECHO.
        ECHO A build error occurred. Stopping at codegen.sln build, configuration %%C.
        GOTO END
    )
    
    SET TEST_BINARY_DIR=%BIN_LOCATION%\AnyCPU\%%C\codegen.test\
    SET TEST_BINARY_PATH=!TEST_BINARY_DIR!codegen.test.dll
    
    mstest /testcontainer:!TEST_BINARY_PATH!
    
    IF !ERRORLEVEL! NEQ 0 ( 
            ECHO.
            ECHO One or more tests have failed. Stopping at codegen.sln build, configuration %%C.
            GOTO END
    )
)

:: DXRT.sln
:: TODO: Enable tests for this solution. The Dxrt tests are a bit different from the codegen ones in that
:: they require being run in an AppContainer. See task #1053.
for %%P in (x86 x64 ARM) DO (
    for %%C in (Debug Release) DO (
        msbuild %GIT_TREE_LOCATION%\dxrt.sln %BUILD_FLAGS% /p:Configuration="%%C" /p:Platform="%%P"
        IF !ERRORLEVEL! NEQ 0 ( 
            ECHO.
            ECHO A build error occurred. Stopping at dxrt.sln build, configuration %%C, platform %%P.
            GOTO END
        )
    )
)

ECHO.
ECHO All builds succeeded. All tests succeeded.

:END

@ENDLOCAL
