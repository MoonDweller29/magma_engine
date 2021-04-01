#pragma once
#include <memory>
#include "magma/vk/Context.h"
#include "magma/vk/validationLayers/DebugMessenger.h"
#include "magma/vk/LogicalDevice.h"
#include "magma/vk/Window.h"
#include "magma/vk/SwapChain.h"
#include "magma/vk/textures/Texture.h"
#include "magma/app/render/GBuffer.h"
#include "magma/app/scene/mesh.h"
#include "magma/app/scene/meshReader.h"
#include "magma/app/scene/directLight.h"
#include "magma/app/render/DepthPass.h"
#include "magma/app/render/MainColorPass.h"
#include "magma/app/render/GBufferResolve.h"
#include "magma/app/render/SwapChainImageSupplier.h"
#include "magma/glm_inc.h"
#include "magma/app/Camera.h"
#include "clock.h"



class App {
public:
    int run();

private:
    uint32_t WIN_WIDTH = 960, WIN_HEIGHT = 540;
    const int MAX_FRAMES_IN_FLIGHT = 2;
    const std::string _buildInfoFilename = "build_info.config";
    std::string _dataPath;
    std::string _texturePath;
    std::string _modelPath;

    MeshReader _meshReader;
    std::vector<Mesh> _scene;
    std::vector<Vertex>   _vertices;
    std::vector<uint32_t> _indices;
    std::unique_ptr<Camera>      _mainCamera;
    std::unique_ptr<DirectLight> _light;

    std::unique_ptr<Context>            _instance;
    std::unique_ptr<DebugMessenger>     _debugMessenger;
    std::unique_ptr<LogicalDevice>      _device;
    std::unique_ptr<Window>             _window;
    Keyboard                           *_keyBoard;
    Mouse                              *_mouse;
    std::unique_ptr<SwapChain>          _swapChain;

    std::vector<vk::Semaphore> _imageAvailableSemaphores;
    size_t                     _currentFrame = 0;

    std::unique_ptr<DepthPass>              _depthPass;
    std::unique_ptr<DepthPass>              _renderShadow;
    std::unique_ptr<MainColorPass>          _mainColorPass;
    std::unique_ptr<GBufferResolve>         _gBufferResolve;
    std::unique_ptr<SwapChainImageSupplier> _swapChainImageSupplier;

    Buffer _vertexBuffer;
    Buffer _indexBuffer;
    Buffer _uniformBuffer;
    Buffer _shadowUniform;
    Buffer _fragmentUniform;
    Buffer _lightSpaceUniform;
    Texture     _texture;
    vk::Sampler _textureSampler;
    Texture     _shadowMap;
    vk::Sampler _shadowMapSampler;
    std::unique_ptr<GBuffer> _gBuffer;
    Texture _mainRenderTarget;

    Clock _global_clock;

    void initWindow();
    void initFromConfig();
    void cleanupSwapChain();
    void recreateSwapChain();
    void createResolutionDependentRenderModules();
    void clearResolutionDependentRenderModules();


    void loadScene();
    void initVulkan();
    void initDevice();
    void createUniformBuffers();
    void updateUniformBuffer(uint32_t currentImage);
    vk::Sampler createDefaultTextureSampler(vk::Filter minFilter, vk::Filter magFilter);
    void createShadowMapTex();
    void createShadowMapResources();
    void updateShadowUniform();
    void createMainRenderTarget();
    void createSyncObjects();
    void mainLoop();

    void drawFrame();

    void cleanUp();
    bool isClosed();
};
