project "VulkanRenderer"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"

	removeplatforms { "WiiU", "3DS" }

	targetdir ("%{wks.location}/Binaries/" .. outputdir)
	objdir ("%{wks.location}/Intermediate/" .. outputdir .. "/%{prj.name}")

	defines {
		"VK_NO_PROTOTYPES",
		"VMA_DYNAMIC_VULKAN_FUNCTIONS"
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
		"%{wks.location}/Engine/Vendor/glm",
		"%{wks.location}/Engine/Vendor/stb",
		"%{wks.location}/Engine/Vendor/stduuid",
		"Vendor/vulkan-headers/include",
		"Vendor/volk",
		"Vendor/vma"
	}
	
	links { "Engine" }
