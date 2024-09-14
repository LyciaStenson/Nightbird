project "flecs"
	kind "StaticLib"
	language "C"
	staticruntime "off"

	targetdir ("%{wks.location}/out/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/out/obj/" .. outputdir .. "/%{prj.name}")

	files
	{
		"**.c",
		"**.h"
	}

	includedirs
	{
		"include"
	}