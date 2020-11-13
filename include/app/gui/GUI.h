#pragma once

#include <imgui.h>
#include <imgui_impl_vulkan.h>
#include <imgui_impl_glfw.h>

class GUI
{
    ImGui_ImplVulkan_InitInfo init_info;
public:
    void SetupImGui();
    void SetupWithVulkan();
};