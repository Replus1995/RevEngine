--Dependencies

VULKAN_SDK = os.getenv("VULKAN_SDK")

IncludeDir = {}

IncludeDir = {}
IncludeDir["glfw"] = "%{wks.location}/../vendor/glfw/include"
IncludeDir["glm"] = "%{wks.location}/../vendor/glm"
IncludeDir["stb"] = "%{wks.location}/../vendor/stb"
IncludeDir["spdlog"] = "%{wks.location}/../vendor/spdlog/include"
IncludeDir["Glad"] = "%{wks.location}/../vendor/Glad/include"
IncludeDir["ImGui"] = "%{wks.location}/../vendor/imgui"
IncludeDir["VulkanSDK"] = "%{VULKAN_SDK}/Include"

LibraryDir = {}
LibraryDir["glfw"] = "%{wks.location}/../vendor/glfw/lib"
LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/Lib"

Library = {}
Library["glfw"] = "%{LibraryDir.glfw}/glfw3.lib"

Library["Vulkan"] = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"
Library["VulkanUtils"] = "%{LibraryDir.VulkanSDK}/VkLayer_utils.lib"

Library["ShaderC_Debug"] = "%{LibraryDir.VulkanSDK}/shaderc_sharedd.lib"
Library["SPIRV_Cross_Debug"] = "%{LibraryDir.VulkanSDK}/spirv-cross-cored.lib"
Library["SPIRV_Cross_GLSL_Debug"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsld.lib"
Library["SPIRV_Tools_Debug"] = "%{LibraryDir.VulkanSDK}/SPIRV-Toolsd.lib"

Library["ShaderC_Release"] = "%{LibraryDir.VulkanSDK}/shaderc_shared.lib"
Library["SPIRV_Cross_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-core.lib"
Library["SPIRV_Cross_GLSL_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsl.lib"

-- Windows
Library["WinSock"] = "Ws2_32.lib"
Library["WinMM"] = "Winmm.lib"
Library["WinVersion"] = "Version.lib"
Library["BCrypt"] = "Bcrypt.lib"