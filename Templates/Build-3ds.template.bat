@echo off
echo Building for 3DS...

set MSYS2_SHELL=C:\devkitPro\msys2\msys2_shell.cmd

if not exist "%MSYS2_SHELL%" (
	echo devkitPro MSYS2 not found at "%MSYS2_SHELL%"
	echo Please ensure devkitPro is installed at C:\devkitPro
	pause
	exit /b 1
)

set PROJECT_DIR=%~dp0
set PROJECT_DIR=%PROJECT_DIR:\=/%

call "%MSYS2_SHELL%" -defterm -here -no-start -msys -c "cd '%PROJECT_DIR%' && mkdir -p Binaries/3ds && mkdir -p Intermediate/3ds && make -f Makefile.3ds %*"

if %errorlevel% neq 0 (
	echo 3DS build failed.
	pause
	exit /b %errorlevel%
)

echo 3DS build successful.
pause
