#pragma once

#include <memory>

#include "magma/vk/textures/TexPyramid.h"
#include "magma/app/render/ssao/DepthGather.h"
#include "magma/vk/vulkan_common.h"

class DepthPyramidPass {
public:
    DepthPyramidPass(LogicalDevice &device, Texture depthTex, int pyramidSize, Queue queue);

    const TexPyramid &getDepthPyramid() const { return _depthPyramid; }

    const CmdSync &draw(
            const std::vector<vk::Semaphore> &waitSemaphores,
            const std::vector<vk::Fence> &waitFences);
private:
    vk::Extent2D _baseExtent;
    TexPyramid   _depthPyramid;
    std::vector<std::unique_ptr<DepthGather>> _gatherModules;

    static vk::Extent2D divideExtentByScale(vk::Extent2D extent, int scaleFactor);
};