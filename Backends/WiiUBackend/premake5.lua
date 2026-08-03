project "WiiUBackend"
	language "C++"
	cppdialect "C++20"
	excludefrombuild "On"

	filter "system:windows"
		kind "StaticLib"
	filter { }

	filter "system:not windows"
		kind "None"
	filter { }

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
