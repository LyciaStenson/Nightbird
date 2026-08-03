project "fastgltf"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"

	pic "On"

	removeconfigurations { "AppDebug", "AppRelease" }

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
