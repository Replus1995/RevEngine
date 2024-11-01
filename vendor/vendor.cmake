include(${CMAKE_CURRENT_LIST_DIR}/../cmake/Common.cmake)

message("========Begin Vendor========")

#========Compile From Source========#
#Glad
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/Glad ${CMAKE_CURRENT_BINARY_DIR}/Glad)
group_target(Glad "vendor")
#glfw
set(GLFW_BUILD_DOCS OFF CACHE BOOL "Disable glfw build docs")
set(GLFW_INSTALL OFF CACHE BOOL "Disable glfw install")
set(GLFW_LIBRARY_TYPE "STATIC")
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/glfw ${CMAKE_CURRENT_BINARY_DIR}/glfw)
group_target(glfw "vendor/GLFW3")
group_target(update_mappings "vendor/GLFW3")
#imgui
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/imgui ${CMAKE_CURRENT_BINARY_DIR}/imgui)
group_target(imgui "vendor")
#physfs
set(PHYSFS_ARCHIVE_GRP OFF CACHE BOOL "Disable physfs GRP")
set(PHYSFS_ARCHIVE_WAD OFF CACHE BOOL "Disable physfs WAD")
set(PHYSFS_ARCHIVE_HOG OFF CACHE BOOL "Disable physfs HOG")
set(PHYSFS_ARCHIVE_WAD OFF CACHE BOOL "Disable physfs WAD")
set(PHYSFS_ARCHIVE_MVL OFF CACHE BOOL "Disable physfs MVL")
set(PHYSFS_ARCHIVE_QPAK OFF CACHE BOOL "Disable physfs QPAK")
set(PHYSFS_ARCHIVE_SLB OFF CACHE BOOL "Disable physfs SLB")
set(PHYSFS_ARCHIVE_ISO9660 OFF CACHE BOOL "Disable physfs ISO9660")
set(PHYSFS_ARCHIVE_VDF OFF CACHE BOOL "Disable physfs VDF")
set(PHYSFS_BUILD_SHARED OFF CACHE BOOL "Disable physfs build shared")
set(PHYSFS_BUILD_TEST OFF CACHE BOOL "Disable physfs build test")
set(PHYSFS_DISABLE_INSTALL TRUE CACHE BOOL "Disable physfs installl")
set(PHYSFS_BUILD_DOCS OFF CACHE BOOL "Disable physfs build docs")
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/physfs ${CMAKE_CURRENT_BINARY_DIR}/physfs)
group_target(physfs-static "vendor")
#cityhash
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/cityhash ${CMAKE_CURRENT_BINARY_DIR}/cityhash)
group_target(cityhash "vendor")

#skaarj1989/FrameGraph
set(FG_BUILD_TEST OFF CACHE BOOL "Disable FrameGraph uint test")
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/FrameGraph ${CMAKE_CURRENT_BINARY_DIR}/FrameGraph)
group_target(FrameGraph "vendor")
#========Compile From Source========#

#========Header Only========#
#glm
set(GLM_BUILD_LIBRARY OFF CACHE BOOL "Disable glm build library")
set(GLM_BUILD_TESTS OFF CACHE BOOL "Disable glm build tests")
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/glm)
#VMA
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/VMA)
#========Header Only========#

#========PreCompiled========#
include(${CMAKE_CURRENT_LIST_DIR}/OpenMesh/OpenMesh.cmake)
#========PreCompiled========#

message("========End Vendor========")