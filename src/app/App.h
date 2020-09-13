#pragma once
#include <memory>
#include "vk/vkInstanceHolder.h"
#include "vk/validationLayers.h"
#include "vk/physicalDevice.h"
#include "vk/logicalDevice.h"
#include "vk/window.h"
#include "vk/swapChain.h"
#include "vk/buffer.h"
#include "vk/texture.h"
#include "scene/mesh.h"
#include "scene/meshReader.h"
#include "scene/directLight.h"
#include "render/depthPass.h"
#include "render/colorPass.h"
#include "glm_inc.h"
#include "camera.h"
#include "clock.h"




class App {
    uint32_t WIN_WIDTH = 960, WIN_HEIGHT = 540;
    const int MAX_FRAMES_IN_FLIGHT = 2;
    const std::string TEXTURE_PATH = "../models/viking_room.png";
    MeshReader meshReader;
    std::vector<Mesh> scene;
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    std::unique_ptr<VkInstanceHolder>     instance;
    std::unique_ptr<DebugMessenger>       debugMessenger;
    std::unique_ptr<PhysicalDevice>       physicalDevice;
    std::unique_ptr<LogicalDevice>  device;
    std::unique_ptr<Window>               window;
    Keyboard *keyBoard;
    Mouse *mouse;
    std::unique_ptr<Camera> mainCamera;
    std::unique_ptr<DirectLight> light;
    std::unique_ptr<SwapChain>            swapChain;
    std::unique_ptr<DepthPass> depthPass;
    std::unique_ptr<ColorPass> colorPass;
    std::unique_ptr<DepthPass> renderShadow;
    Buffer vertexBuffer;
    Buffer indexBuffer;
    Buffer uniformBuffer;
    Buffer shadowUniform;
    Buffer fragmentUniform;
    Buffer lightSpaceUniform;
    std::vector<VkSemaphore> imageAvailableSemaphores;
    size_t currentFrame = 0;
    Texture texture;
    VkSampler textureSampler;
    VkSampler shadowMapSampler;
    Texture depthTex;
    Texture shadowMap;

    Clock global_clock;

	void initWindow();
    void cleanupSwapChain();
	void recreateSwapChain();
	void loadScene();

    void initVulkan();
    void createUniformBuffers();
    void updateUniformBuffer(uint32_t currentImage);
    void createTexture();
    void createTextureSampler();
    void createShadowMapSampler();
    void createShadowMapTex();
    void createShadowMapResources();
    void updateShadowUniform();
    void createDepthResources();
    void createSyncObjects();
    void mainLoop();
    void drawFrame();

    void cleanUp();

    bool isClosed();
public:
    void run();
};