--Dependencies

VULKAN_SDK = os.getenv("VULKAN_SDK")

IncludeDir = {}

IncludeDir = {}
IncludeDir["glfw"] = "%{wks.location}/../vendor/glfw/include"
IncludeDir["glm"] = "%{wks.location}/../vendor/glm"
IncludeDir["spdlog"] = "%{wks.location}/../vendor/spdlog/include"
IncludeDir["Glad"] = "%{wks.location}/../vendor/Glad/include"
IncludeDir["ImGui"] = "%{wks.location}/../vendor/imgui"
IncludeDir["entt"] = "%{wks.location}/../vendor/entt/single_include"
IncludeDir["stb"] = "%{wks.location}/../vendor/stb"
IncludeDir["jsonhpp"] = "%{wks.location}/../vendor/jsonhpp"
IncludeDir["tinygltf"] = "%{wks.location}/../vendor/tinygltf"

IncludeDir["VulkanSDK"] = "%{VULKAN_SDK}/Include"

LibraryDir = {}
LibraryDir["glfw"] = "%{wks.location}/../vendor/glfw/lib"
LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/Lib"

Library = {}
Library["glfw"] = "%{LibraryDir.glfw}/glfw3.lib"

Library["Vulkan"] = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"
Library["VulkanUtils"] = "%{LibraryDir.VulkanSDK}/VkLayer_utils.lib"

Library["ShaderC"] = "%{LibraryDir.VulkanSDK}/shaderc_shared.lib"
Library["SPIRV_Cross"] = "%{LibraryDir.VulkanSDK}/spirv-cross-core.lib"
Library["SPIRV_Cross_GLSL"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsl.lib"
Library["SPIRV_Tools"] = "%{LibraryDir.VulkanSDK}/SPIRV-Tools.lib"



-- Windows
Library["WinSock"] = "Ws2_32.lib"
Library["WinMM"] = "Winmm.lib"
Library["WinVersion"] = "Version.lib"
Library["BCrypt"] = "Bcrypt.lib"