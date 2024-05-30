project "Sandbox"
    location "%{wks.location}/Sandbox"
    kind "ConsoleApp"
    language "C++"

    targetdir ("%{wks.location}/bin/" .. outputdir)
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "src/**.h",
        "src/**.cpp"
    }

    includedirs
    {
        "%{IncludeDir.spdlog}",
        "%{wks.location}/../Pine/src"
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
            "PE_PLATFORM_WINDOWS"
        }

    filter "configurations:Debug"
        defines { "PE_DEBUG", "PE_ENABLE_ASSERTS" }
        buildoptions  "/MDd"
        symbols "On"

    filter "configurations:Release"
        defines { "PE_RELEASE", "PE_ENABLE_ASSERTS" }
        buildoptions  "/MD"
        optimize "On"

    filter "configurations:Dist"
        defines "PE_DIST"
        buildoptions  "/MD"
        optimize "On"