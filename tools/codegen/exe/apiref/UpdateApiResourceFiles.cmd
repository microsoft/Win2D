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

SET GIT_TREE_LOCATION=%~dp0..\..\..\..

IF NOT "%1"=="" (
   SET GIT_TREE_LOCATION=%1
)

SET COPYRIGHT_BANNER_FILENAME=%GIT_TREE_LOCATION%\tools\copyright\copyright.txt


:: Copy the Direct2D API description files.

SET SOURCE_DIR=%SDXROOT%\windows\wgi\codegen\publish\
SET DEST_DIR=%GIT_TREE_LOCATION%\tools\codegen\exe\apiref\

SET FILE_LIST=^
    D2DTypes.xml ^
    D2DTypes2.xml ^
    D2DTypes3.xml ^
    D2DTypes4.xml ^
    D2DEffectAuthor.xml

CALL :UPDATE_FILES


:: Copy the DirectImage effect description files.

SET SOURCE_DIR=%SDXROOT%\windows\wgi\dimage\Effects\
SET DEST_DIR=%GIT_TREE_LOCATION%\tools\codegen\exe\apiref\effects\

SET FILE_LIST=^
    2DAffineTransform.xml ^
    3DTransform.xml ^
    Arithmetic.xml ^
    Atlas.xml ^
    Blend.xml ^
    Border.xml ^
    Brightness.xml ^
    ChromaKey.xml ^
    Clip.xml ^
    ColorMatrix.xml ^
    Composite.xml ^
    Contrast.xml ^
    Convolve.xml ^
    DiffuseDistant.xml ^
    DiffusePoint.xml ^
    DiffuseSpot.xml ^
    DirectionalBlur.xml ^
    DiscreteTransfer.xml ^
    DisplacementMap.xml ^
    DpiCompensation.xml ^
    EdgeDetection.xml ^
    Emboss.xml ^
    Exposure.xml ^
    Flood.xml ^
    GammaTransfer.xml ^
    GaussianBlur.xml ^
    Grayscale.xml ^
    HighlightsShadows.xml ^
    Histogram.xml ^
    HueRotation.xml ^
    HueToRgb.xml ^
    Invert.xml ^
    LinearTransfer.xml ^
    LuminanceToAlpha.xml ^
    Morphology.xml ^
    OpacityMetadata.xml ^
    Posterize.xml ^
    Premultiply.xml ^
    RgbToHue.xml ^
    Saturation.xml ^
    Scale.xml ^
    Sepia.xml ^
    shadow.xml ^
    Sharpen.xml ^
    SpecularDistant.xml ^
    SpecularPoint.xml ^
    SpecularSpot.xml ^
    Straighten.xml ^
    TableTransfer.xml ^
    TemperatureTint.xml ^
    Tile.xml ^
    Turbulence.xml ^
    UnPremultiply.xml ^
    Vignette.xml

CALL :UPDATE_FILES

GOTO END


:UPDATE_FILES
    IF NOT EXIST %SOURCE_DIR% (
        ECHO The source location %SOURCE_DIR% was not available.
        ECHO Please ensure your Windows source environment is configured with the 
        ECHO correct project.
        GOTO END
    )

    PUSHD %SOURCE_DIR%
    
    sd sync %SOURCE_DIR%*.xml

    :: This script skips over the first line of the API files, which contain the
    :: <xml> declaration. This is because XML files aren't allowed to lead with
    :: a comment if there is an <xml> declaration.

    FOR %%A IN (%FILE_LIST%) DO ( 
        SET FILENAME=%%A
        ECHO Updating !FILENAME!
        
        SET SOURCE_PATH=%SOURCE_DIR%!FILENAME!
        SET DEST_PATH=%DEST_DIR%!FILENAME!
        
        ECHO ^<?xml version="1.0"?^>> !DEST_PATH!
        ECHO ^<^^!-->> !DEST_PATH!
        MORE %COPYRIGHT_BANNER_FILENAME% >> !DEST_PATH!
        ECHO --^>>> !DEST_PATH!
        ECHO.>> !DEST_PATH!

        MORE +1 !SOURCE_PATH! >> !DEST_PATH!
    )

    POPD
    EXIT /B


:END

@ENDLOCAL
