project "glad"
    kind "StaticLib"
    language "C"
    staticruntime "off"

    targetdir ("%{wks.location}/out/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/out/obj/" .. outputdir .. "/%{prj.name}")

    files
    {
        "include/glad/gl.h",
        "include/KHR/khrplatform.h",
        "src/gl.c"
    }

    includedirs
    {
        "include"
    }

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"