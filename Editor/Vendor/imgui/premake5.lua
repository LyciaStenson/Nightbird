project "imgui"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"

	removeconfigurations { "AppDebug", "AppRelease" }
	removeplatforms { "WiiU", "3DS" }

	targetdir ("%{wks.location}/Intermediate/" .. outputdir)
	objdir ("%{wks.location}/Intermediate/" .. outputdir .. "/imgui")

	files { "**.h", "**.cpp" }

	includedirs { "." }

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
