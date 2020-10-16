#include "app/UI.h"

void UI::SetupImGui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
}

void UI::SetupWithVulkan() {
    //ImGui_ImplGlfw_InitForVulkan(window, true);
    //init_info_.Instance = g_Instance;
    //init_info_.PhysicalDevice = g_PhysicalDevice;
    //init_info_.Device = g_Device;
    //init_info_.QueueFamily = g_QueueFamily;
    //init_info_.Queue = g_Queue;
    //init_info_.PipelineCache = g_PipelineCache;
    //init_info_.DescriptorPool = g_DescriptorPool;
    init_info_.Allocator = nullptr;
    //init_info_.MinImageCount = g_MinImageCount;
    //init_info_.ImageCount = wd->ImageCount;
    //init_info_.CheckVkResultFn = check_vk_result;
    //ImGui_ImplVulkan_Init(&init_info_, wd->RenderPass);
}