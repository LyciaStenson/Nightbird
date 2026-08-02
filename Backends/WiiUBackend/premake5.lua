project "WiiUBackend"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"

	filter { "action:gmake*" }
		disablewarnings { "invalid-offsetof" }
	filter { }

	removeconfigurations { "EditorDebug", "EditorRelease" }
	removeplatforms { "x64", "x86", "ARM64", "3DS" }

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
		"C:/devkitPro/wut/include"
	}
	
	links { "Engine" }
	
	filter { "platforms:x64 or x86 or ARM64" }
		removefiles {
			"Source/Private/**.cpp"
		}
	filter { }
