/**
 * @file GUI.h
 * @author Nastormo
 * @brief Header file that contains the GUI declaration
 * @version 0.1
 * @date 2021-01-22
 */
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

/**
 * @brief Graphics user interface class
 */
class GUI
{
private:
    Window& _window;
    VkInstanceHolder& _instance;
    PhysicalDevice& _physicalDevice;
    LogicalDevice& _device;

    std::vector<FrameBuffer> _frameBuffers;
    VkRenderPass _renderPass;
    VkDescriptorPool _descriptorPool;
    CommandBufferArr _commandBuffers;
    CmdSync _renderFinished;
    int _imgCount;
    VkExtent2D _extent;

    bool _iAnotherWindow;

    void createRenderPass(SwapChain &swapChain);
    void createFrameBuffers(SwapChain &swapChain);
    void createCommandBuffers();
    void createDescriptorPool();
    void recordCmdBuffers(uint32_t i);
    void createInterface();

public:
/**
 * @brief Construct a new GUI object
 * 
 * @param window Sample Window class
 * @param instance Sample VkInstanceHolder class
 * @param physicalDevice Sample PhysicalDevice class
 * @param device Sample LogicalDevice class
 * @param swapChain Sample SwapChain class
 */
    GUI(Window &window, VkInstanceHolder &instance, PhysicalDevice &physicalDevice,
        LogicalDevice &device, SwapChain &swapChain);
/**
 * @brief Destroy the GUI object
 */
    ~GUI();
/**
 * @brief Clear FrameBuffers and RenderPass
 */
    void cleanup();
/**
 * @brief Init ImGui enviroment
 */
    void setupImGui();
/**
 * @brief Link ImGui with Vulkan engine
 */
    void setupWithVulkan();
/**
 * @brief Load Fonts to GPU
 */
    void uploadFonts();
/**
 * @brief Recreating resources when recreating SwapChain
 * 
 * @param swapChain New SwapChain
 */
    void recreateSwapChain(SwapChain &swapChain);
/**
 * @brief Filling command buffers at startup
 */
    void initCmdBuffers();
/**
 * @brief Draw Frame
 * 
 * @param i Number image in SwapChain
 * @param waitFence [unuse] Fence needed for synchronization
 * @return CmdSync Fance draw GUI
 */
    CmdSync draw(uint32_t i, VkFence waitFence);
};