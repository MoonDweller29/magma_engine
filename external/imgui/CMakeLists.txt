cmake_minimum_required (VERSION 3.14)

project(imgui)

set(SOURCE_LIB 
    imgui/imgui.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_widgets.cpp
)		

add_library(imgui STATIC ${SOURCE_LIB})