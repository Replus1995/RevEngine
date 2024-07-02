project "Rev"
    location "%{wks.location}/Rev"
    kind "SharedLib"
    language "C++"
    unitybuild "true"

    targetdir ("%{wks.location}/bin/" .. outputdir)
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "src/**.h",
        "src/**.inl",
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
        "%{IncludeDir.stb}",
        "%{IncludeDir.entt}",
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
        "%{Library.Vulkan}",
        "%{Library.ShaderC}",
        "%{Library.SPIRV_Cross}",
        "%{Library.SPIRV_Cross_GLSL}"
    }

    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"

        defines
        {
            "RE_PLATFORM_WINDOWS",
            "RE_BUILD_DLL",
            "GLFW_INCLUDE_NONE"
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
