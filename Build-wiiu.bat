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

echo Compiling shaders (WSL required)
wsl bash -c "cd $(wslpath '%PROJECT_DIR%') && mkdir -p Intermediate/wiiu && Tools/CafeGLSL/glslcompiler.elf -vs Backends/WiiUBackend/Shaders/Shader.vert -ps Backends/WiiUBackend/Shaders/Shader.frag -o Intermediate/wiiu/Shader.gsh"

if %errorlevel% neq 0 (
	echo Shader compilation failed. Ensure WSL is installed and Tools/CafeGLSL/glslcompiler.elf has execution permission.
	pause
	exit /b %errorlevel%
)

set PROJECT_DIR=%PROJECT_DIR:\=/%

call "%MSYS2_SHELL%" -defterm -here -no-start -msys -c "cd '%PROJECT_DIR%' && make -f Makefile.wiiu %*"

if %errorlevel% neq 0 (
	echo Wii U build failed.
	pause
	exit /b %errorlevel%
)

echo Wii U build successful.
pause
