@echo off
pushd %~dp0
msbuild /p:Configuration=Release /p:Platform=AnyCPU /v:q /nologo exportsample.csproj 
%~dp0..\..\bin\windowsanycpu\release\exportsample\exportsample %*
popd

