project "Engine"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"

	pic "On"

	filter { "configurations:EditorDebug or EditorRelease" }
		kind "SharedLib"
	filter { }

	filter { "configurations:AppDebug or AppRelease" }
		kind "StaticLib"
	filter { }

	disablewarnings { "invalid-offsetof" }

	targetdir ("%{wks.location}/Binaries/" .. outputdir)
	objdir ("%{wks.location}/Intermediate/" .. outputdir .. "/%{prj.name}")

	files {
		"Source/Public/**.h",
		"Source/Private/**.h",
		"Source/Private/**.cpp",
	}

	includedirs {
		"Source/Public",
		"Source/Private",
		"Vendor/glm",
		"Vendor/stb",
		"Vendor/stduuid"
	}

	defines { "NB_ENGINE_BUILD" }
