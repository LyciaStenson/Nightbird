workspace "Nightbird"
	configurations { "EditorDebug", "EditorRelease", "AppDebug", "AppRelease" }
	platforms { "Desktop", "WiiU", "3DS" }
	defaultplatform "Desktop"
	startproject "Editor"

	exceptionhandling "Off"
	rtti "Off"

	defines { "TOML_COMPILER_HAS_EXCEPTIONS=0" }

	filter { "platforms:Desktop" }
		architecture "x86_64"
	filter { }

	filter { "configurations:EditorDebug or AppDebug" }
		defines { "DEBUG" }
		symbols "On"
		optimize "Off"
		runtime "Debug"
	filter { }

	filter { "configurations:EditorRelease or AppRelease" }
		defines { "NDEBUG" }
		optimize "On"
		runtime "Release"
	filter { }

	outputdir = "%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}"

group "Dependencies"
	include "Backends/Libraries/GlfwPlatform/Vendor/glfw"
	include "Editor/Vendor/fastgltf"
	include "Editor/Vendor/imgui"
group ""

group "Nightbird"
	include "Engine"
	include "App"
group ""

group "Nightbird/Backends"
	include "Backends/GlfwVulkanBackend"
	include "Backends/WiiUBackend"
	include "Backends/3DSBackend"
group ""

group "Nightbird/Backends/Libraries"
	include "Backends/Libraries/GlfwPlatform"
	include "Backends/Libraries/VulkanRenderer"
group ""

group "Nightbird/Editor"
	include "Editor"
group ""

group "Nightbird/Editor/Backends"
	include "Editor/Backends/EditorGlfwVulkanBackend"
group ""

group "Nightbird/Editor/Backends/Libraries"
	include "Editor/Backends/Libraries/EditorGlfwPlatform"
	include "Editor/Backends/Libraries/EditorVulkanRenderer"
group ""
