@echo off
call "%~dp0GenerateProject.bat" fresh %*
exit /b %errorlevel%
