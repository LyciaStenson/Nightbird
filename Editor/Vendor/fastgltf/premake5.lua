project "fastgltf"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"

	pic "On"

	removeconfigurations { "AppDebug", "AppRelease" }
	removeplatforms { "WiiU", "3DS" }

	targetdir ("%{wks.location}/Intermediate/" .. outputdir)
	objdir ("%{wks.location}/Intermediate/" .. outputdir .. "/fastgltf")

	files
	{
		"include/**.hpp",
		"src/**.cpp"
	}

	includedirs
	{
		"include"
	}
