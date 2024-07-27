cmake_minimum_required(VERSION 3.19.0)
project(Glad)

add_library(Glad
    STATIC
    "include/glad/gl.h"
    "include/KHR/khrplatform.h"
    "src/gl.c"
)

target_include_directories(Glad
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)