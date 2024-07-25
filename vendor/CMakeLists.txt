cmake_minimum_required(VERSION 3.19.0)
include(${CMAKE_CURRENT_LIST_DIR}/../cmake/Common.cmake)
project(vendor)

#========Compile From Source========#
#Glad
add_subdirectory(Glad ${CMAKE_CURRENT_BINARY_DIR}/Glad)
group_target(Glad "vendor")
#imgui
add_subdirectory(imgui ${CMAKE_CURRENT_BINARY_DIR}/imgui)
group_target(imgui "vendor")
#========Compile From Source========#