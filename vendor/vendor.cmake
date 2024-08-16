include(${CMAKE_CURRENT_LIST_DIR}/../cmake/Common.cmake)

message("========Begin Vendor========")

#========Compile From Source========#
#Glad
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/Glad ${CMAKE_CURRENT_BINARY_DIR}/Glad)
group_target(Glad "vendor")
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
#vk-bootstrap
set(VK_BOOTSTRAP_VULKAN_HEADER_DIR $ENV{VULKAN_SDK}/Include)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/vk-bootstrap ${CMAKE_CURRENT_BINARY_DIR}/vk-bootstrap)
group_target(vk-bootstrap "vendor")
#========Compile From Source========#

#========PreCompiled========#
include(${CMAKE_CURRENT_LIST_DIR}/GLFW/glfw3.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/OpenMesh/OpenMesh.cmake)
#========PreCompiled========#

message("========End Vendor========")