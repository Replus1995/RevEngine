add_library(OpenMeshCore STATIC IMPORTED GLOBAL)
add_library(OpenMeshTools STATIC IMPORTED GLOBAL)
if(WIN32)
    set_target_properties(OpenMeshCore PROPERTIES
        IMPORTED_LOCATION_DEBUG "${CMAKE_CURRENT_LIST_DIR}/lib/OpenMeshCored.lib"   
        IMPORTED_LOCATION_RELEASE "${CMAKE_CURRENT_LIST_DIR}/lib/OpenMeshCore.lib"
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/include"
        INTERFACE_COMPILE_OPTIONS "/bigobj"
    )

    set_target_properties(OpenMeshTools PROPERTIES
        IMPORTED_LOCATION_DEBUG "${CMAKE_CURRENT_LIST_DIR}/lib/OpenMeshToolsd.lib"   
        IMPORTED_LOCATION_RELEASE "${CMAKE_CURRENT_LIST_DIR}/lib/OpenMeshTools.lib"
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/include"
        INTERFACE_LINK_LIBRARIES "OpenMeshCore"
    )
endif()