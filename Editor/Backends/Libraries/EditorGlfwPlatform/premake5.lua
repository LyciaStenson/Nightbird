project "EditorGlfwPlatform"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"

	removeconfigurations { "AppDebug", "AppRelease" }
	removeplatforms { "WiiU", "3DS" }

	targetdir ("%{wks.location}/Binaries/" .. outputdir)
	objdir ("%{wks.location}/Intermediate/" .. outputdir .. "/%{prj.name}")

	defines {
		"NB_EDITOR_BUILD",
	}

	files {
		"Source/Public/**.h",
		"Source/Private/**.h",
		"Source/Private/**.cpp",
		"Vendor/imgui/**.h",
		"Vendor/imgui/**.cpp"
	}

	includedirs {
		"Source/Public",
		"Source/Private",
		"Vendor/imgui",
		"%{wks.location}/Engine/Source/Public",
		"%{wks.location}/Editor/Source/Public",
		"%{wks.location}/Engine/Vendor/glm",
		"%{wks.location}/Engine/Vendor/stb",
		"%{wks.location}/Engine/Vendor/stduuid",
		"%{wks.location}/Editor/Vendor/imgui",
		"%{wks.location}/Backends/Libraries/GlfwPlatform/Source/Public",
		"%{wks.location}/Backends/Libraries/GlfwPlatform/Vendor/glfw/include",
		"%{wks.location}/Backends/Libraries/GlfwPlatform/Vendor/miniaudio",
		"%{wks.location}/Backends/Libraries/VulkanRenderer/Vendor/vulkan-headers/include"
	}

	links { "GlfwPlatform", "GLFW", "imgui" }
