#include "magma/app/render/ssao/DepthPyramidPass.h"

DepthPyramidPass::DepthPyramidPass(LogicalDevice &device, Texture depthTex, int pyramidSize, Queue queue) :
    _baseExtent(toExtent2D(depthTex.getInfo()->imageInfo.extent)),
    _depthPyramid("depth_pyramid", device.getTextureManager(),
                  divideExtentByScale(_baseExtent, 3), 3, pyramidSize,
                  vk::Format::eR16G16B16A16Sfloat,
                  vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled,
                  vk::ImageAspectFlagBits::eColor)
{
    _gatherModules.push_back(
        std::make_unique<DepthGather>(device.getDevice(), depthTex, _depthPyramid[0], queue, true)
    );
    for (int i = 1; i < pyramidSize; ++i) {
        _gatherModules.push_back(
            std::make_unique<DepthGather>(device.getDevice(), _depthPyramid[i-1], _depthPyramid[i], queue, false)
        );
    }
}

const CmdSync &DepthPyramidPass::draw(
        const std::vector<vk::Semaphore> &waitSemaphores,
        const std::vector<vk::Fence> &waitFences
) {
    _gatherModules[0]->draw(waitSemaphores, waitFences);
    for (int i = 1; i < _depthPyramid.getSize(); ++i) {
        _gatherModules[i]->draw({ _gatherModules[i-1]->getSync().getSemaphore() }, {});
    }

    return _gatherModules.back()->getSync();
}

vk::Extent2D DepthPyramidPass::divideExtentByScale(vk::Extent2D extent, int scaleFactor) {
    return vk::Extent2D((extent.width + scaleFactor - 1) / scaleFactor,
                        (extent.height + scaleFactor - 1) / scaleFactor);
}
