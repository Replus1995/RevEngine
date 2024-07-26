cmake_minimum_required(VERSION 3.19.0)
project(imgui)

add_library(imgui
    STATIC
    "imconfig.h"
    "imgui.h"
    "imgui_internal.h"
    "imstb_rectpack.h"
    "imstb_textedit.h"
    "imstb_truetype.h"
    "imgui.cpp"
    "imgui_draw.cpp"
    "imgui_widgets.cpp"
    "imgui_tables.cpp"
    "imgui_demo.cpp"
)

target_include_directories(imgui
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)