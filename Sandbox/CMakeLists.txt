cmake_minimum_required(VERSION 3.19.0)
project(Sandbox)

file(GLOB_RECURSE SANDBOX_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Source/*)
group_source(SANDBOX_SRC_FILES)

add_library(Sandbox
STATIC
    ${SANDBOX_SRC_FILES}
)

target_link_libraries(Sandbox
Public
    Rev
)