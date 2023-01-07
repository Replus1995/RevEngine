workspace "Pine"
    architecture "x64"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

--Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "Pine/vendor/GLFW/include"
IncludeDir["Glad"] = "Pine/vendor/Glad/include"

LibDir = {}
LibDir["GLFW"] = "Pine/vendor/GLFW/lib"

include "Pine/vendor/Glad"

project "Pine"
    location"Pine"
    kind "SharedLib"
    language "C++"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "pinepch.h"
    pchsource "Pine/src/pinepch.cpp"

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

    includedirs
    {
        "%{prj.name}/src",
        "%{prj.name}/vendor/spdlog/include",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.Glad}"
    }

    libdirs
    {
        "%{LibDir.GLFW}"
    }

    links
    {
        "Glad",
        "glfw3",
        "opengl32"
    }

    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"

        defines
        {
            "PINE_PLATFORM_WINDOWS",
            "PINE_BUILD_DLL",
            "GLFW_INCLUDE_NONE"
        }

        postbuildcommands
        {
            ("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox")

        }

    filter "configurations:Debug"
        defines "PINE_DEBUG"
        buildoptions  "/MDd"
        symbols "On"

    filter "configurations:Release"
        defines "PINE_RELEASE"
        buildoptions  "/MD"
        optimize "On"

    filter "configurations:Dist"
        defines "PINE_DIST"
        buildoptions  "/MD"
        optimize "On"

project "Sandbox"
    location "Sandbox"
    kind "ConsoleApp"
    language "C++"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

    includedirs
    {
        "Pine/vendor/spdlog/include",
        "Pine/src"
    }

    links
    {
        "Pine"
    }

    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"

        defines
        {
            "PINE_PLATFORM_WINDOWS"
        }

    filter "configurations:Debug"
        defines "PINE_DEBUG"
        buildoptions  "/MDd"
        symbols "On"

    filter "configurations:Release"
        defines "PINE_RELEASE"
        buildoptions  "/MD"
        optimize "On"

    filter "configurations:Dist"
        defines "PINE_DIST"
        buildoptions  "/MD"
        optimize "On"