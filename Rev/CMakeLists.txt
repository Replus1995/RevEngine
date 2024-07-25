cmake_minimum_required(VERSION 3.19.0)
project(Rev)

file(GLOB_RECURSE REV_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Source/*)
group_source(REV_SRC_FILES)

add_library(Rev
STATIC
    ${REV_SRC_FILES}
)

target_compile_definitions(Rev
PUBLIC
    $<$<CONFIG:Debug>:RE_DEBUG>
    $<$<CONFIG:Release>:RE_RELEASE>
    NOMINMAX #spdlog
    _SCL_SECURE_NO_WARNINGS #spdlog
PRIVATE
    RE_BUILD_DLL
    GLFW_INCLUDE_NONE
    _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING
)


target_include_directories(Rev
PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Public
    ${VENDOR_ROOT}/spdlog/include
    ${VENDOR_ROOT}/entt/single_include
    ${VENDOR_ROOT}/glm
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Source
    $ENV{VULKAN_SDK}/Include
    ${VENDOR_ROOT}/jsonhpp
    ${VENDOR_ROOT}/stb
    ${VENDOR_ROOT}/tinygltf
    ${VENDOR_ROOT}/GLFW/include
)

target_link_directories(Rev
PUBLIC
PRIVATE
    ${VENDOR_ROOT}/GLFW/lib
    $ENV{VULKAN_SDK}/Lib
)

if(WIN32)
    target_compile_definitions(Rev
    PUBLIC
        RE_PLATFORM_WINDOWS
    )

    target_link_libraries(Rev
    PRIVATE
        Glad
        imgui
        glfw3
        vulkan-1
        shaderc_shared
        spirv-cross-core
        spirv-cross-glsl
    )
endif()

