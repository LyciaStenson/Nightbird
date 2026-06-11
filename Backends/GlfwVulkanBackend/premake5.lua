local shaderDir = "%{wks.location}/Backends/Libraries/VulkanRenderer/Shaders/"
local outDir = "%{wks.location}/Binaries/" .. outputdir .. "/"

project "GlfwVulkanBackend"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"

	removeplatforms { "WiiU", "3DS" }

	targetdir (outDir)
	objdir ("%{wks.location}/Intermediate/" .. outputdir .. "/%{prj.name}")

	defines {
		"VK_NO_PROTOTYPES",
		"VMA_DYNAMIC_VULKAN_FUNCTIONS",
		"GLFW_INCLUDE_VULKAN"
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
		"%{wks.location}/Backends/Libraries/GlfwPlatform/Source/Public",
		"%{wks.location}/Backends/Libraries/GlfwPlatform/Vendor/glfw/include",
		"%{wks.location}/Backends/Libraries/GlfwPlatform/Vendor/miniaudio",
		"%{wks.location}/Backends/Libraries/VulkanRenderer/Source/Public",
		"%{wks.location}/Backends/Libraries/VulkanRenderer/Vendor/vulkan-headers/include",
		"%{wks.location}/Backends/Libraries/VulkanRenderer/Vendor/volk",
		"%{wks.location}/Backends/Libraries/VulkanRenderer/Vendor/vma"
	}

	links { "GlfwPlatform", "VulkanRenderer" }

	filter { "system:windows" }
		postbuildcommands {
			"%{wks.location}/Tools/glslc/glslc.exe " .. shaderDir .. "Pbr.vert -o " .. outDir .. "Pbr.vert.spv",
			"%{wks.location}/Tools/glslc/glslc.exe " .. shaderDir .. "Pbr.frag -o " .. outDir .. "Pbr.frag.spv",
			"%{wks.location}/Tools/glslc/glslc.exe " .. shaderDir .. "Skybox.vert -o " .. outDir .. "Skybox.vert.spv",
			"%{wks.location}/Tools/glslc/glslc.exe " .. shaderDir .. "Skybox.frag -o " .. outDir .. "Skybox.frag.spv"
		}

	filter { "system:linux" }
		postbuildcommands {
			"%{wks.location}/Tools/glslc/glslc " .. shaderDir .. "Pbr.vert -o " .. outDir .. "Pbr.vert.spv",
			"%{wks.location}/Tools/glslc/glslc " .. shaderDir .. "Pbr.frag -o " .. outDir .. "Pbr.frag.spv",
			"%{wks.location}/Tools/glslc/glslc " .. shaderDir .. "Skybox.vert -o " .. outDir .. "Skybox.vert.spv",
			"%{wks.location}/Tools/glslc/glslc " .. shaderDir .. "Skybox.frag -o " .. outDir .. "Skybox.frag.spv"
		}

	filter { }
