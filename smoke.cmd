@echo off

msbuild "%~dp0Win2D.proj" /v:m /maxcpucount /nr:false /p:BuildPlatforms="x64;AnyCPU" /p:BuildConfigurations=Debug /p:BuildPhone=false /p:BuildDocs=false %*
