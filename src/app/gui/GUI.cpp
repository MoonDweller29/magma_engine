#include "app/gui/GUI.h"

#include "vk/vulkan_common.h"

void check_vk_result(VkResult err)
{
    if (err != VK_SUCCESS)
       throw std::runtime_error("Error in init with vulkan");
}

GUI::GUI(Window &window, 
    VkInstanceHolder &instance, 
    PhysicalDevice &physicalDevice,
    LogicalDevice &device, 
    SwapChain &swapChain) 
    : _window(window), 
    _instance(instance), 
    _physicalDevice(physicalDevice),
    _device(device), 
    _swapChain(swapChain) {
    _iCommandBuffers.allocate(_device.handler(), _device.getGraphicsCmdPool(), _swapChain.imgCount());
    _iRenderFinished.create(_device.handler());
    createDescriptorPool();
    createRenderPass();
    createFrameBuffers();
}

GUI::~GUI() {
    _iFrameBuffers.clear();
    vkDestroyRenderPass(_device.handler(), _iRenderPass, nullptr);
    vkFreeCommandBuffers(_device.handler(), _device.getGraphicsCmdPool(),
        _iCommandBuffers.size(), _iCommandBuffers.data());
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    vkDestroyDescriptorPool(_device.handler(), _iDescriptorPool, nullptr);
}

void GUI::createRenderPass() {
    VkAttachmentDescription attachment = {};
    attachment.format = _swapChain.getImageFormat();
    attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    attachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
    attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference color_attachment = {};
    color_attachment.attachment = 0;
    color_attachment.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment;

    VkSubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    info.attachmentCount = 1;
    info.pAttachments = &attachment;
    info.subpassCount = 1;
    info.pSubpasses = &subpass;
    info.dependencyCount = 1;
    info.pDependencies = &dependency;

    if (vkCreateRenderPass(_device.handler(), &info, nullptr, &_iRenderPass) != VK_SUCCESS) {
        throw std::runtime_error("Could not create Dear ImGui's render pass");
    }
}

void GUI::createFrameBuffers() {
    std::vector<VkImageView> attachments = _swapChain.getImageViews();
    VkExtent2D extent = _swapChain.getExtent();
    for (uint32_t i = 0; i < attachments.size(); i++) {
        std::vector<VkImageView> currImage = { attachments[i] };
        _iFrameBuffers.emplace_back(currImage, extent, _iRenderPass, _device.handler());
    }
}

void GUI::createDescriptorPool() {
    VkDescriptorPoolSize pool_sizes[] =
    {
        { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
    };
    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
    pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
    pool_info.pPoolSizes = pool_sizes;
    vkCreateDescriptorPool(_device.handler(), &pool_info, nullptr, &_iDescriptorPool);
}

void GUI::recordCmdBuffers(uint32_t i) {
    _iCommandBuffers.resetCmdBuf(i);
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::ShowDemoWindow();
    ImGui::Render();
    VkCommandBuffer cmdBuf = _iCommandBuffers.beginCmdBuf(i);
    {
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = _iRenderPass;
        renderPassInfo.framebuffer = _iFrameBuffers[i].getHandler();

        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = _swapChain.getExtent();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {0.0f, 0.0f, 0.0f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(cmdBuf, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        {
            ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmdBuf);
        }
        vkCmdEndRenderPass(cmdBuf);
    }
    _iCommandBuffers.endCmdBuf(i);
}

void GUI::SetupImGui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
}

void GUI::SetupWithVulkan() {
    ImGui_ImplGlfw_InitForVulkan(_window.getGLFWp(), true);
    ImGui_ImplVulkan_InitInfo _initInfo = {};
    _initInfo.Instance = _instance.get();
    _initInfo.PhysicalDevice = _physicalDevice.device();
    _initInfo.Device = _device.handler();
    _initInfo.QueueFamily = 1; //May be wrong
    _initInfo.Queue = _device.getGraphicsQueue();
    _initInfo.PipelineCache = VK_NULL_HANDLE;
    _initInfo.DescriptorPool = _iDescriptorPool;
    _initInfo.Allocator = nullptr;
    _initInfo.MinImageCount = _swapChain.imgCount(); //I don't know what mean this var
    _initInfo.ImageCount = _swapChain.imgCount();
    _initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    _initInfo.CheckVkResultFn = check_vk_result;
    ImGui_ImplVulkan_Init(&_initInfo, _iRenderPass);
}

void GUI::uploadFonts() {
    SingleTimeCommandBuffer tmpCmdBuffer(_device.handler(), 
        _device.getGraphicsCmdPool(), _device.getGraphicsQueue());
    VkCommandBuffer commandBuffer = tmpCmdBuffer.startRecording();
    ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
    tmpCmdBuffer.endRecordingAndSubmit();  
}

void GUI::recreateSwapChain() {
    //TODO
}

void GUI::initCmdBuffers() {
    for (size_t i = 0; i < _swapChain.imgCount(); ++i)
    {
        recordCmdBuffers(i);
    }
}

CmdSync GUI::draw(uint32_t i, VkFence waitFence) {

    //vkWaitForFences(_device.handler(), 1, &waitFence, VK_TRUE, UINT64_MAX);


    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &_iCommandBuffers[i];

    vkResetFences(_device.handler(), 1, &_iRenderFinished.fence);

    VkResult result = vkQueueSubmit(_device.getGraphicsQueue(), 1, &submitInfo, _iRenderFinished.fence);
    vk_check_err(result, "failed to submit draw command buffer!");

    recordCmdBuffers(i);

    return _iRenderFinished;
}