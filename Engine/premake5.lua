project "Engine"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"

	pic "On"

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

	filter { "configurations:EditorDebug or EditorRelease" }
		defines { "NB_EDITOR_BUILD" }
	filter { }
