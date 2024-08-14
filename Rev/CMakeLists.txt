cmake_minimum_required(VERSION 3.19.0)
project(Rev)

file(GLOB_RECURSE REV_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Source/*)
group_source(REV_SRC_FILES)

add_library(Rev
SHARED
    ${REV_SRC_FILES}
)

set_target_properties(Rev PROPERTIES UNITY_BUILD TRUE)

target_compile_definitions(Rev
PUBLIC
    $<$<CONFIG:Debug>:RE_DEBUG>
    $<$<CONFIG:Release>:RE_RELEASE>
    _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING
    NOMINMAX #spdlog
    _SCL_SECURE_NO_WARNINGS #spdlog
PRIVATE
    REV_BUILD_DLL
    GLFW_INCLUDE_NONE
)


target_include_directories(Rev
PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Public
    ${VENDOR_ROOT}/spdlog/include
    ${VENDOR_ROOT}/entt/single_include
    ${VENDOR_ROOT}/glm
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Private
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Platform
    $ENV{VULKAN_SDK}/Include
    ${VENDOR_ROOT}/jsonhpp
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
        glfw3
        Glad
        vulkan-1
        shaderc_shared
        spirv-cross-core
        spirv-cross-glsl
        imgui
    )
endif()

