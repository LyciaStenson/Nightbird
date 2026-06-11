@echo off
echo Building for Wii U...

set MSYS2_SHELL=C:\devkitPro\msys2\msys2_shell.cmd

if not exist "%MSYS2_SHELL%" (
	echo devkitPro MSYS2 not found at "%MSYS2_SHELL%"
	echo Please ensure devkitPro is installed at C:\devkitPro
	pause
	exit /b 1
)

set PROJECT_DIR=%~dp0

set PROJECT_DIR=%PROJECT_DIR:\=/%

call "%MSYS2_SHELL%" -defterm -here -no-start -msys -c "cd '%PROJECT_DIR%' && make -f Makefile.wiiu %*"

if %errorlevel% neq 0 (
	echo Wii U build failed.
	pause
	exit /b %errorlevel%
)

echo Wii U build successful.
pause
