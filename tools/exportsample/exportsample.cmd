@echo off
msbuild /p:Configuration=Release /p:Platform=AnyCPU /t:Build /v:q /nologo %~dp0\exportsample.csproj
%~dp0..\..\bin\windowsanycpu\release\exportsample\exportsample %*
