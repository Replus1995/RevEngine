cmake_minimum_required(VERSION 3.19.0)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Common.cmake)

set(CMAKE_CXX_STANDARD 20)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMakeTargets")

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_BINARY_DIR}/Debug/Lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_BINARY_DIR}/Debug/Lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_BINARY_DIR}/Debug/Bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_BINARY_DIR}/Release/Lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_BINARY_DIR}/Release/Lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_BINARY_DIR}/Release/Bin)
#set(CMAKE_UNITY_BUILD TRUE)

project(RevEngine)
if(MSVC)
    add_compile_options(
        #$<$<CONFIG:Debug>:/MDd>
        #$<$<CONFIG:Release>:/MD>
        "/MD"
        "/std:c++20"
    )
endif()

#vender
set(VENDOR_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/vendor)
include(${CMAKE_CURRENT_SOURCE_DIR}/vendor/vendor.cmake)

#Rev
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Rev ${CMAKE_CURRENT_BINARY_DIR}/Rev)
group_target(Rev "Core")

#SandBox
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Sandbox ${CMAKE_CURRENT_BINARY_DIR}/Sandbox)
group_target(Sandbox "Misc")

if(MSVC)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Sandbox)
endif()