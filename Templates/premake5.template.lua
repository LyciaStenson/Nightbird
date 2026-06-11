local enginePath = os.getenv("NIGHTBIRD_PATH")
assert(enginePath, "NIGHTBIRD_PATH environment variable not found.")

enginePath = enginePath:gsub("\\", "/")
if enginePath:sub(-1) ~= "/" then
	enginePath = enginePath .. "/"
end

workspace "%PROJECT_NAME%"
	configurations { "EditorDebug", "EditorRelease", "AppDebug", "AppRelease" }
	platforms { "Desktop", "WiiU", "3DS" }
	defaultplatform "Desktop"
	startproject "Editor"

	exceptionhandling "Off"
	rtti "Off"

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

group "Nightbird"

project "EngineSource"
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
		enginePath .. "Engine/Source/Public/**.h",
		enginePath .. "Engine/Source/Private/**.h",
		enginePath .. "Engine/Source/Private/**.cpp",
	}
	
	includedirs {
		enginePath .. "Engine/Source/Public",
		enginePath .. "Engine/Source/Private",
		enginePath .. "Engine/Vendor/glm",
		enginePath .. "Engine/Vendor/stb",
		enginePath .. "Engine/Vendor/stduuid"
	}

project "Editor"
	kind "None"

	removeconfigurations { "AppDebug", "AppRelease" }
	removeplatforms { "WiiU", "3DS" }

	targetdir ("%{wks.location}/Binaries/" .. outputdir)
	objdir ("%{wks.location}/Intermediate/" .. outputdir)

	filter { "system:windows" }
		debugcommand (enginePath .. "Binaries/" .. outputdir .. "/Editor.exe")
	filter { }

	filter { "system:linux" }
		debugcommand (enginePath .. "Binaries/" .. outputdir .. "/Editor")
	filter { }

	debugdir (enginePath .. "Binaries/" .. outputdir)

	debugargs { "%{wks.location}/%PROJECT_NAME%.nproject" }

	dependson { "%PROJECT_NAME%" }

group ""

project "%PROJECT_NAME%"
	language "C++"
	cppdialect "C++20"

	pic "On"

	filter { "configurations:EditorDebug or EditorRelease" }
		kind "SharedLib"
	filter { }

	filter { "configurations:AppDebug or AppRelease" }
		kind "ConsoleApp"
	filter { }

	local outBinDir = "%{wks.location}/Binaries/" .. outputdir

	targetdir (outBinDir)
	objdir ("%{wks.location}/Intermediate/" .. outputdir)

	debugdir (outBinDir)

	defines { "PROJECT_BUILD" }

	files {
		"Source/**.h",
		"Source/**.cpp",
		enginePath .. "Engine/Source/Public/Core/ProjectInit.cpp"
	}

	includedirs {
		"Source/Public",
		enginePath .. "Engine/Source/Public",
		enginePath .. "Engine/Vendor/glm",
		enginePath .. "Engine/Vendor/stb",
		enginePath .. "Engine/Vendor/stduuid",
	}
	
	libdirs { enginePath .. "Binaries/" .. outputdir }

	filter { "configurations:EditorDebug or EditorRelease" }
		links { "Engine" }
		defines { "NB_EDITOR_BUILD" }
	filter { }

	filter { "configurations:AppDebug or AppRelease", "platforms:Desktop" }
		links { "App", "GlfwVulkanBackend", "GlfwPlatform", "VulkanRenderer", "glfw", "Engine" }

		local engineBinaries = enginePath .. "Binaries/%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}/"
		local projectBinaries = "%{wks.location}/Binaries/%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}/"

		postbuildcommands {
			"{COPYFILE} " .. engineBinaries .. "Pbr.vert.spv " .. projectBinaries .. "Pbr.vert.spv",
			"{COPYFILE} " .. engineBinaries .. "Pbr.frag.spv " .. projectBinaries .. "Pbr.frag.spv",
			"{COPYFILE} " .. engineBinaries .. "Skybox.vert.spv " .. projectBinaries .. "Skybox.vert.spv",
			"{COPYFILE} " .. engineBinaries .. "Skybox.frag.spv " .. projectBinaries .. "Skybox.frag.spv"
		}
	filter { }

	filter { "configurations:AppDebug or AppRelease", "platforms:WiiU" }
		links { "App", "WiiUBackend", "Engine" }
	filter { }

	filter { "configurations:AppDebug or AppRelease", "platforms:3DS" }
		links { "App", "3DSBackend", "Engine" }
	filter { }
