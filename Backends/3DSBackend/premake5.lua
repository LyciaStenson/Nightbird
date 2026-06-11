project "3DSBackend"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"

	removeconfigurations { "EditorDebug", "EditorRelease" }
	removeplatforms { "Desktop", "WiiU" }

	targetdir ("%{wks.location}/Binaries/" .. outputdir)
	objdir ("%{wks.location}/Intermediate/" .. outputdir .. "/%{prj.name}")

	defines { }
	
	files {
		"Source/Public/**.h",
		"Source/Private/**.h",
		"Source/Private/**.cpp"
	}

	includedirs {
		"Source/Public",
		"Source/Private",
		"%{wks.location}/Engine/Source/Public",
		"%{wks.location}/Engine/Vendor/glm",
		"%{wks.location}/Engine/Vendor/stb",
		"%{wks.location}/Engine/Vendor/stduuid",
		"C:/devkitPro/libctru/include"
	}
	
	links { "Engine" }
	
	filter { "platforms:Desktop" }
		removefiles {
			"Source/Private/**.cpp"
		}
	filter { }
