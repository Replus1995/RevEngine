cmake_minimum_required(VERSION 3.19.0)
project(Sandbox)

file(GLOB_RECURSE SANDBOX_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Source/*)
file(GLOB_RECURSE SANDBOX_SHADER_FILES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/Shaders/*.rsf
    ${CMAKE_CURRENT_SOURCE_DIR}/Shaders/*.rsh
)
group_source(SANDBOX_SRC_FILES)
group_source(SANDBOX_SHADER_FILES)

set_source_files_properties(${SANDBOX_SHADER_FILES} PROPERTIES HEADER_FILE_ONLY TRUE)

add_executable(Sandbox
    ${SANDBOX_SRC_FILES}
    ${SANDBOX_SHADER_FILES}
)

target_link_libraries(Sandbox
    Rev
)

if(MSVC)
    set_property(TARGET Sandbox PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
endif()
