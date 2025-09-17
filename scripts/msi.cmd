@echo off
cd "%~1"
start /wait msiexec /i "%~2" /qn /norestart /log install.log
