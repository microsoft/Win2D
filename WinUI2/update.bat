@echo off

if "%1" == "" (
    echo Usage: update {path to unzipped docs}
    goto END
)

set DOCDIR=%1

if not exist %DOCDIR% (
    echo Not found: %DOCDIR%
    goto END
)

robocopy %DOCDIR% . /mir /np /nc /ns /nfl /ndl /njh /njs /xf update.bat /xd .git

git add --all .

:END
