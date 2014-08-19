@ECHO OFF
@SETLOCAL
@SETLOCAL ENABLEDELAYEDEXPANSION

:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::
:: Description:
::     This script syncs the last API config files from the
::     Windows source tree, prepends a copyright banner,
::     and copies them to your Git enlistment. 
::
::     This is a necessary step for ensuring that codegen is 
::     kept up to date, despite a couple of its Windows-centric 
::     dependencies living in a very separate location.
::
:: Usage:
::     UpdateApiResourceFiles.cmd [path]
::
::     where [path] is the absolute path to your 
::     local Git repository. This parameter is optional.
::
::     If [path] is not specified, the script will infer
::     your Git repository path from its location (works
::     so long as it hasn't been copied someplace else).
::
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

IF NOT DEFINED SDXROOT (
    ECHO This script relies on Windows source control environment features.
    ECHO Please ensure this script is run from a Windows enlistment.
    GOTO END
)

IF "%1"=="/?" (
    ECHO Usage: %0 [path]
    ECHO    where [path] is the path to your local Git repository.
    ECHO    Example: UpdateApiResourceFiles.cmd C:\Src
    GOTO END
)

SET GIT_TREE_LOCATION=%~dp0..\..\..

IF NOT "%1"=="" (
   SET GIT_TREE_LOCATION=%1
)

SET SOURCE_DIR=%SDXROOT%\windows\wgi\codegen\publish\
SET DEST_DIR=%GIT_TREE_LOCATION%\tools\codegen\exe\
SET COPYRIGHT_BANNER_FILENAME=%GIT_TREE_LOCATION%\tools\copyright\copyright.txt

IF NOT EXIST %SOURCE_DIR% (
    ECHO The source location %SOURCE_DIR% was not available.
    ECHO Please ensure your Windows source environment is configured with the 
    ECHO correct project.
    GOTO END
)

PUSHD %SOURCE_DIR%

:: This script skips over the first line of the API files, which contain the
:: <xml> declaration. This is because XML files aren't allowed to lead with
:: a comment if there is an <xml> declaration.

FOR %%A IN (D2DTypes.xml D2DTypes2.xml D2DTypes3.xml D2DEffectAuthor.xml) DO ( 
    SET FILENAME=%%A
    
    SET SOURCE_PATH=%SOURCE_DIR%!FILENAME!
    SET DEST_PATH=%DEST_DIR%!FILENAME!
    
    sd sync !SOURCE_PATH!
    
    ECHO ^<?xml version="1.0"?^>> !DEST_PATH!
    ECHO ^<^^!-->> !DEST_PATH!
    MORE %COPYRIGHT_BANNER_FILENAME% >> !DEST_PATH!
    ECHO --^>>> !DEST_PATH!
    ECHO.>> !DEST_PATH!

    MORE +1 !SOURCE_PATH! >> !DEST_PATH!
)

POPD

:END

@ENDLOCAL
