cmake_minimum_required(VERSION 3.19.0)
project(Rev)

file(GLOB_RECURSE REV_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Source/*)
file(GLOB_RECURSE REV_SHADER_FILES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/Shaders/*.rsf
    ${CMAKE_CURRENT_SOURCE_DIR}/Shaders/*.rsh
)
group_source(REV_SRC_FILES)
group_source(REV_SHADER_FILES)

set_source_files_properties(${REV_SHADER_FILES} PROPERTIES HEADER_FILE_ONLY TRUE)

add_library(Rev
SHARED
    ${REV_SRC_FILES}
    ${REV_SHADER_FILES}
)

set_target_properties(Rev PROPERTIES UNITY_BUILD OFF)

target_compile_definitions(Rev
PUBLIC
    $<$<CONFIG:Debug>:REV_DEBUG>
    $<$<CONFIG:Release>:REV_RELEASE>
    _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING
    NOMINMAX #spdlog
    _SCL_SECURE_NO_WARNINGS #spdlog
PRIVATE
    REV_BUILD_DLL
    REV_ENGINE_SHADER_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/Shaders\"
    GLFW_INCLUDE_NONE
)


target_include_directories(Rev
PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Public
    ${VENDOR_ROOT}/spdlog/include
    ${VENDOR_ROOT}/entt/single_include
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Private
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Platform
    $ENV{VULKAN_SDK}/Include
    ${VENDOR_ROOT}/stb
    ${VENDOR_ROOT}/tinygltf
)

target_link_directories(Rev
PUBLIC
PRIVATE
    $ENV{VULKAN_SDK}/Lib
)

if(WIN32)
    target_compile_definitions(Rev
    PUBLIC
        REV_PLATFORM_WINDOWS
    )

    target_link_libraries(Rev
    PRIVATE
        physfs-static
        glfw
        vulkan-1
        shaderc_shared
        spirv-cross-core
        spirv-cross-glsl
        spirv-cross-hlsl
        spirv-cross-reflect
        GPUOpen::VulkanMemoryAllocator
        imgui
        cityhash
    )
endif()

