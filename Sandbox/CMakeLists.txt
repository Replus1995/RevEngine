cmake_minimum_required(VERSION 3.19.0)
project(Sandbox)

file(GLOB_RECURSE SANDBOX_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Source/*)
group_source(SANDBOX_SRC_FILES)

add_executable(Sandbox
    ${SANDBOX_SRC_FILES}
)

target_link_libraries(Sandbox
    Rev
)

if(MSVC)
    set_property(TARGET Sandbox PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
endif()