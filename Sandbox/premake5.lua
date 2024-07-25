project "Sandbox"
    location "%{wks.location}/Sandbox"
    kind "ConsoleApp"
    language "C++"

    targetdir ("%{wks.location}/bin/" .. outputdir)
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "Source/**.h",
        "Source/**.cpp"
    }

    includedirs
    {
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.entt}",
        "%{IncludeDir.Rev}"
    }

    links
    {
        "Rev"
    }

    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"
        debugdir "."

        defines
        {
            "RE_PLATFORM_WINDOWS"
        }

    filter "configurations:Debug"
        defines { "RE_DEBUG", "RE_ENABLE_ASSERTS" }
        buildoptions  "/MD"
        symbols "On"

    filter "configurations:Release"
        defines { "RE_RELEASE", "RE_ENABLE_ASSERTS" }
        buildoptions  "/MD"
        optimize "On"

    filter "configurations:Dist"
        defines "RE_DIST"
        buildoptions  "/MD"
        optimize "On"