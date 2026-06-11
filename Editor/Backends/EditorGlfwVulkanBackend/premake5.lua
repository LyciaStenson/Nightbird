project "EditorGlfwVulkanBackend"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"

	removeconfigurations { "AppDebug", "AppRelease" }
	removeplatforms { "WiiU", "3DS" }

	targetdir ("%{wks.location}/Binaries/" .. outputdir)
	objdir ("%{wks.location}/Intermediate/" .. outputdir .. "/%{prj.name}")

	defines {
		"VK_NO_PROTOTYPES",
		"IMGUI_IMPL_VULKAN_NO_PROTOTYPES",
		"VMA_DYNAMIC_VULKAN_FUNCTIONS",
		"GLFW_INCLUDE_VULKAN",
		"NB_EDITOR_BUILD"
	}

	files {
		"Source/Public/**.h",
		"Source/Private/**.h",
		"Source/Private/**.cpp"
	}

	includedirs {
		"Source/Public",
		"Source/Private",
		"%{wks.location}/Engine/Source/Public",
		"%{wks.location}/Editor/Source/Public",
		"%{wks.location}/Engine/Vendor/glm",
		"%{wks.location}/Engine/Vendor/stb",
		"%{wks.location}/Engine/Vendor/stduuid",
		"%{wks.location}/Editor/Vendor/imgui",
		"%{wks.location}/Backends/Libraries/GlfwPlatform/Source/Public",
		"%{wks.location}/Backends/Libraries/GlfwPlatform/Vendor/glfw/include",
		"%{wks.location}/Backends/Libraries/GlfwPlatform/Vendor/miniaudio",
		"%{wks.location}/Backends/Libraries/VulkanRenderer/Source/Public",
		"%{wks.location}/Backends/Libraries/VulkanRenderer/Vendor/vulkan-headers/include",
		"%{wks.location}/Backends/Libraries/VulkanRenderer/Vendor/volk",
		"%{wks.location}/Backends/Libraries/VulkanRenderer/Vendor/vma",
		"%{wks.location}/Editor/Backends/Libraries/EditorGlfwPlatform/Source/Public",
		"%{wks.location}/Editor/Backends/Libraries/EditorVulkanRenderer/Source/Public"
	}

	links { "GlfwPlatform", "VulkanRenderer", "EditorGlfwPlatform", "EditorVulkanRenderer" }
