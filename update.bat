@echo off

if "%1" == "" (
    echo Usage: update {buildname}  // eg: update 1803.16018
    goto END
)

set DOCDIR=\\edge-svcs\Release\Win2D\Win2D-release\%1\bin\docs

if not exist %DOCDIR% (
    echo Not found: %DOCDIR%
    goto END
)

robocopy %DOCDIR% . /mir /np /nc /ns /nfl /ndl /njh /njs /xf update.bat /xd .git

git add --all .

:END
