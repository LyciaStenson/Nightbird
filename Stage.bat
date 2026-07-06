@echo off

setlocal enabledelayedexpansion

set SRC=.
set DST=Staging

if exist "Staging" (
	echo Cleaning previous staging folder...
	rmdir /s /q "Staging"
)

echo Copying Nightbird (excluding Intermediate, Binaries, VS artifacts)
robocopy "%SRC%" "%DST%" /E ^
	/XD Intermediate Binaries Staging .vs .git obj x64 x86 Debug Release ^
	/XF *.vcxproj *.vcxproj.filters *.vcxproj.user *.sln *.suo *.user .gitattributes .gitignore Stage.bat WindowsInstaller.nsi NightbirdInstaller.exe

rem robocopy exit codes 0-7 are success/informational; 8+ indicate failure
if %ERRORLEVEL% GEQ 8 (
	echo Main tree copy failed with error %ERRORLEVEL%.
	exit /b 1
)

echo Copying template .gitignore for Blank project template
if not exist "%SRC%\Templates\Projects\Blank\.gitignore" (
	echo Failed to find Blank template project .gitignore.
	exit /b 1
)
copy /Y "%SRC%\Templates\Projects\Blank\.gitignore" "%DST%\Templates\Projects\Blank\.gitignore" >nul
if !ERRORLEVEL! NEQ 0 (
	echo Failed to copy Blank template project .gigignore with error !ERRORLEVEL!.
	exit /b 1
)

echo Copying Debug Windows Binaries (keeping .pdb)
for %%C in (AppDebug EditorDebug) do (
	robocopy "%SRC%\Binaries\windows-x86_64\%%C" "%DST%\Binaries\windows-x86_64\%%C" /E ^
		/XD .vs obj ^
		/XF *.idb *.exp *.ilk
	if !ERRORLEVEL! GEQ 8 (
		echo Binaries copy failed for %%C with error !ERRORLEVEL!.
		exit /b 1
	)
)

echo Copying Release Windows Binaries (stripping .pdb)
for %%C in (AppRelease EditorRelease) do (
	robocopy "%SRC%\Binaries\windows-x86_64\%%C" "%DST%\Binaries\windows-x86_64\%%C" /E ^
		/XD .vs obj ^
		/XF *.idb *.exp *.ilk *.pdb
	if !ERRORLEVEL! GEQ 8 (
		echo Binaries copy failed for %%C with error !ERRORLEVEL!.
		exit /b 1
	)
)

echo Copying Wii U Binaries
robocopy "%SRC%\Binaries\wiiu" "%DST%\Binaries\wiiu" /E ^
	/XD .vs obj ^
	/XF *.idb *.exp *.ilk *.pdb
if !ERRORLEVEL! GEQ 8 (
	echo Binaries copy failed for Wii U with error !ERRORLEVEL!.
	exit /b 1
)

echo Copying 3DS Binaries
robocopy "%SRC%\Binaries\3ds" "%DST%\Binaries\3ds" /E ^
	/XD .vs obj ^
	/XF *.idb *.exp *.ilk *.pdb
if !ERRORLEVEL! GEQ 8 (
	echo Binaries copy failed for 3DS with error !ERRORLEVEL!.
	exit /b 1
)

echo Staging complete
exit /b 0
