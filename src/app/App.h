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
#include "vk/buffer.h"
#include "glm_inc.h"
#include "camera.h"
#include "clock.h"


struct Vertex {
    glm::vec2 pos;
    glm::vec3 color;

    static std::vector<VkVertexInputBindingDescription> getBindingDescription();
    static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
};

class App {
    uint32_t WIN_WIDTH = 800, WIN_HEIGHT = 450;
    const int MAX_FRAMES_IN_FLIGHT = 2;

    std::unique_ptr<VkInstanceHolder>     instance;
    std::unique_ptr<DebugMessenger>       debugMessenger;
    std::unique_ptr<PhysicalDevice>       physicalDevice;
    std::unique_ptr<LogicalDevice>  device;
    std::unique_ptr<Window>               window;
    Keyboard *keyBoard;
    Mouse *mouse;
    std::unique_ptr<Camera> mainCamera;
    std::unique_ptr<SwapChain>            swapChain;
    std::unique_ptr<RenderPass>           renderPass;
    std::unique_ptr<GraphicsPipeline>     graphicsPipeline;
    CommandBufferArr commandBuffers;
    Buffer vertexBuffer;
    Buffer indexBuffer;
    std::vector<Buffer> uniformBuffers;
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    std::vector<VkFence> imagesInFlight;
    size_t currentFrame = 0;

    std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
    VkDescriptorPool descriptorPool;
    std::vector<VkDescriptorSet> descriptorSets;

    Clock global_clock;

	void initWindow();
    void cleanupSwapChain();
	void recreateSwapChain();

    void initVulkan();
    void createDescriptorSetLayout();
    void createUniformBuffers();
    void updateUniformBuffer(uint32_t currentImage);
    void createTextureImage();
    void createSyncObjects();
    void createDescriptorPool();
    void createDescriptorSets();
    void mainLoop();
    void drawFrame();

    void cleanUp();

    bool isClosed();
public:
    void run();
};