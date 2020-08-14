#pragma once
#include <memory>
#include "vk/vkInstanceHolder.h"
#include "vk/validationLayers.h"
#include "vk/physicalDevice.h"
#include "vk/logicalDevice.h"
#include "vk/window.h"
#include "vk/swapChain.h"

class App {
    uint32_t WIN_WIDTH = 800, WIN_HEIGHT = 600;

    std::unique_ptr<VkInstanceHolder> instance;
    std::unique_ptr<DebugMessenger> debugMessenger;
    std::unique_ptr<PhysicalDevice> physicalDevice;
    std::unique_ptr<LogicalDeviceHolder> device;
    std::unique_ptr<Window> window;
    std::unique_ptr<SwapChain> swapChain;

	void initWindow();

    void initVulkan();
    void mainLoop();

    void cleanUp();

    bool isClosed();
public:
    void run();
};