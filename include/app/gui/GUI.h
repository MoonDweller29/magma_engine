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

    std::vector<FrameBuffer> _iFrameBuffers;
    VkRenderPass _iRenderPass;
    VkDescriptorPool _iDescriptorPool;
    CommandBufferArr _iCommandBuffers;
    CmdSync _iRenderFinished;
    int _imgCount;
    VkExtent2D _extent;

    void createRenderPass(SwapChain &swapChain);
    void createFrameBuffers(SwapChain &swapChain);
    void createCommandBuffers();
    void createDescriptorPool();
    void recordCmdBuffers(uint32_t i);

public:
    GUI(Window &window, VkInstanceHolder &instance, PhysicalDevice &physicalDevice,
        LogicalDevice &device, SwapChain &swapChain);
    ~GUI();
    void cleanup();
    void setupImGui();
    void setupWithVulkan();
    void uploadFonts();
    void recreateSwapChain(SwapChain &swapChain, uint32_t width, uint32_t height);
    void initCmdBuffers();
    CmdSync draw(uint32_t i, VkFence waitFence);
};