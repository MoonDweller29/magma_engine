#pragma once
#include <memory>
#include "vk/vkInstanceHolder.h"
#include "vk/validationLayers.h"
#include "vk/physicalDevice.h"
#include "vk/logicalDevice.h"
#include "vk/window.h"
#include "vk/swapChain.h"
#include "vk/renderPass.h"
#include "vk/graphicsPipeline.h"
#include "vk/commandPool.h"
#include "vk/commandBuffer.h"

class App {
    uint32_t WIN_WIDTH = 800, WIN_HEIGHT = 600;
    const int MAX_FRAMES_IN_FLIGHT = 2;

    std::unique_ptr<VkInstanceHolder>     instance;
    std::unique_ptr<DebugMessenger>       debugMessenger;
    std::unique_ptr<PhysicalDevice>       physicalDevice;
    std::unique_ptr<LogicalDeviceHolder>  device;
    std::unique_ptr<Window>               window;
    std::unique_ptr<SwapChain>            swapChain;
    std::unique_ptr<RenderPass>           renderPass;
    std::unique_ptr<GraphicsPipeline>     graphicsPipeline;
    std::unique_ptr<CommandPool>          commandPool;
    CommandBufferArr commandBuffers;
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    std::vector<VkFence> imagesInFlight;
    size_t currentFrame = 0;

	void initWindow();
    void cleanupSwapChain();
	void recreateSwapChain();

    void initVulkan();
    void createSyncObjects();
    void mainLoop();
    void drawFrame();

    void cleanUp();

    bool isClosed();
public:
    void run();
};