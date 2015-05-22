@echo off
msbuild /p:Configuration=Release /p:Platform=AnyCPU /v:q /nologo %~dp0\exportsample.csproj 
%~dp0..\..\bin\windowsanycpu\release\exportsample\exportsample %*

