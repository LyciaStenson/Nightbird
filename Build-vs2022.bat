@echo off
echo Building Visual Studio 2022 project files.

"%~dp0Tools/Premake/premake5.exe" vs2022

if %errorlevel% neq 0 (
	echo Failed to build Visual Studio 2022 project files.
	pause
	exit /b %errorlevel%
)

echo Visual Studio 2022 project files built successfully.
pause
