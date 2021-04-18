#include "magma/app/render/blur/Gauss2D.h"

Gauss2D::Gauss2D(vk::Device device, Texture inpTex, Texture tempTex, Texture outTex, Queue queue) :
    _gaussVertical(device, inpTex, tempTex, 0, queue),
    _gaussHorizontal(device, tempTex, outTex, 1, queue)
{}

const CmdSync &Gauss2D::draw(
        const std::vector<vk::Semaphore> &waitSemaphores,
        const std::vector<vk::Fence> &waitFences
) {
    const CmdSync &gaussVerticalSync = _gaussVertical.draw(waitSemaphores, waitFences);
    return  _gaussHorizontal.draw({ gaussVerticalSync.getSemaphore() }, {});
}