@echo off

if not "%1" == "" (
    set DOCDIR=%1
) else (
    set DOCDIR=\\grfxaltbld2\Win2D
)

robocopy %DOCDIR%\docs . /mir /np /nc /ns /nfl /ndl /njh /njs /xf update.bat /xd .git

git add --all .
