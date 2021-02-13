#pragma once
#include <memory>
#include "magma/vk/Context.h"
#include "magma/vk/validationLayers/DebugMessenger.h"
#include "magma/vk/physicalDevice/PhysicalDevice.h"
#include "magma/vk/logicalDevice.h"
#include "magma/vk/Window.h"
#include "magma/vk/swapChain.h"
#include "magma/vk/buffer.h"
#include "magma/vk/textures/Texture.h"
#include "magma/app/scene/mesh.h"
#include "magma/app/scene/meshReader.h"
#include "magma/app/scene/directLight.h"
#include "magma/app/render/depthPass.h"
#include "magma/app/render/colorPass.h"
#include "magma/glm_inc.h"
#include "camera.h"
#include "clock.h"




class App {
public:
    int run();

private:
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

    std::unique_ptr<Context>              instance;
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
};
