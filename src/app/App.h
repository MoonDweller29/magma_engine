#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <memory>
#include "vk/vkInstanceHolder.h"
#include "vk/validationLayers.h"

class App {
public:
    void run();

private:
    const uint32_t WIN_WIDTH = 800, WIN_HEIGHT = 600;
    GLFWwindow* window;
    std::unique_ptr<VkInstanceHolder> instance;
    std::unique_ptr<DebugMessenger> debugMessenger;
	
	void initWindow();
    void initVulkan();

    void mainLoop();

    void cleanUp();
    bool isClosed();
};