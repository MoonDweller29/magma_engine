cmake_minimum_required (VERSION 3.14)

project(imgui)

set(SOURCE_LIB 
    imgui/imgui.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_widgets.cpp
    imgui/examples/imgui_impl_vulkan.cpp
    imgui/examples/imgui_impl_glfw.cpp
)

add_library(imgui STATIC ${SOURCE_LIB})

target_include_directories(imgui
    PRIVATE ../glfw/include
    PUBLIC imgui
    PUBLIC imgui/examples
)