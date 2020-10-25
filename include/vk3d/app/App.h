#pragma once
#include <memory>
#include "vk3d/vk/vkInstanceHolder.h"
#include "vk3d/vk/validationLayers.h"
#include "vk3d/vk/physicalDevice.h"
#include "vk3d/vk/logicalDevice.h"
#include "vk3d/vk/window.h"
#include "vk3d/vk/swapChain.h"
#include "vk3d/vk/buffer.h"
#include "vk3d/vk/texture.h"
#include "vk3d/app/scene/mesh.h"
#include "vk3d/app/scene/meshReader.h"
#include "vk3d/app/scene/directLight.h"
#include "vk3d/app/render/depthPass.h"
#include "vk3d/app/render/colorPass.h"
#include "vk3d/glm_inc.h"
#include "camera.h"
#include "clock.h"




class App {
    uint32_t WIN_WIDTH = 960, WIN_HEIGHT = 540;
    const int MAX_FRAMES_IN_FLIGHT = 2;
    const std::string buildInfoFilename = "build_info.config";
    std::string dataPath;
    std::string texturePath;
    std::string modelPath;
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
	void initFromConfig();
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