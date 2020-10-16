#pragma once

#include <imgui/imgui.h>
#include <imgui/examples/imgui_impl_vulkan.h>
#include <imgui/examples/imgui_impl_glfw.h>

class UI
{
    ImGui_ImplVulkan_InitInfo init_info_;
public:
    void SetupImGui();
    void SetupWithVulkan();
};