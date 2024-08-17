add_library(glfw3 STATIC IMPORTED GLOBAL)
if(WIN32)
    set_target_properties(glfw3 PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/lib/glfw3.lib"
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/include"
    )
endif()
