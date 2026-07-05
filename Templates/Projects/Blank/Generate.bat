@echo off
setlocal

set CONFIG=%1
if "%CONFIG%"=="" set CONFIG=editordebug

set PLATFORM=%2
if "%PLATFORM%"=="" set PLATFORM=desktop

if /i "%CONFIG%"=="editordebug" (
	set CONFIG_DIR=EditorDebug
) else if /i "%CONFIG%"=="editorrelease" (
	set CONFIG_DIR=EditorRelease
) else if /i "%CONFIG%"=="appdebug" (
	set CONFIG_DIR=AppDebug
) else if /i "%CONFIG%"=="apprelease" (
	set CONFIG_DIR=AppRelease
) else (
	echo Unknown config: %CONFIG%
	pause
	exit /b 1
)

:: Build path
set ENGINE_BINARIES=%NIGHTBIRD_PATH%\Binaries\windows-x86_64\%CONFIG_DIR%

set PROJECT_DIR=%cd%

set NPROJECT_FILE=

for %%f in (*.nproject) do (
	set NPROJECT_FILE=%%f
	goto :found_project
)

:found_project
if "%NPROJECT_FILE%"=="" (
	echo No .nproject found in %cd%
	pause
	exit /b 1
)

echo Launching editor with project: %NPROJECT_FILE%

pushd "%ENGINE_BINARIES%"
Editor.exe "%PROJECT_DIR%\%NPROJECT_FILE%" --generate
popd

endlocal
pause