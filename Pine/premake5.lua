project "Pine"
    location "%{wks.location}/Pine"
    kind "SharedLib"
    language "C++"

    targetdir ("%{wks.location}/bin/" .. outputdir)
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "pinepch.h"
    pchsource "src/pinepch.cpp"

    files
    {
        "src/**.h",
        "src/**.cpp"
    }

    includedirs
    {
        "src",
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.glfw}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.Glad}",
        "%{IncludeDir.VulkanSDK}",
    }

    libdirs
    {
        "%{LibraryDir.glfw}",
        "%{LibraryDir.VulkanSDK}"
    }

    links
    {
        "Glad",
        "ImGui",
        "opengl32",
        "%{Library.glfw}",
        "%{Library.Vulkan}"
    }

    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"

        defines
        {
            "PE_PLATFORM_WINDOWS",
            "PE_BUILD_DLL",
            "GLFW_INCLUDE_NONE"
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
