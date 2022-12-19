dir /s

del /f /s /q build\pkges

@REM Keep the githash from the obj folder since the nuspec requires it
copy obj\Win2d.githash.txt Win2d.githash.txt
del /f /s /q obj
mkdir obj
move Win2d.githash.txt obj\Win2d.githash.txt

del /f /s /q .git