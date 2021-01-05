#pragma once

#include <imgui.h>
#include <imgui_impl_vulkan.h>
#include <imgui_impl_glfw.h>

#include "vk/vkInstanceHolder.h"
#include "vk/physicalDevice.h"
#include "vk/logicalDevice.h"
#include "vk/window.h"
#include "vk/swapChain.h"
#include "vk/cmdSync.h"
#include "vk/commandBuffer.h"

class GUI
{
    Window& _window;
    VkInstanceHolder& _instance;
    PhysicalDevice& _physicalDevice;
    LogicalDevice& _device;
    SwapChain& _swapChain;

    std::vector<FrameBuffer> _iFrameBuffers;
    VkRenderPass _iRenderPass;
    VkDescriptorPool _iDescriptorPool;
    CommandBufferArr _iCommandBuffers;
    CmdSync _iRenderFinished;

    void createRenderPass();
    void createFrameBuffers();
    void createCommandBuffers();
    void createDescriptorPool();
    void recordCmdBuffers(uint32_t i);

public:
    GUI(Window &window, VkInstanceHolder &instance, PhysicalDevice &physicalDevice,
        LogicalDevice &device, SwapChain &swapChain);
    ~GUI();
    void SetupImGui();
    void SetupWithVulkan();
    void uploadFonts();
    void recreateSwapChain();
    void initCmdBuffers();
    CmdSync draw(uint32_t i, VkFence waitFence);
};