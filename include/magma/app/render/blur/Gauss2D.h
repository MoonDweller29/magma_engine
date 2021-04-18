#pragma once

#include "magma/app/render/blur/Gauss1D.h"

class Gauss2D {
public:
    Gauss2D(vk::Device device, Texture inpTex, Texture tempTex, Texture outTex, Queue queue);

    const CmdSync &getSync() const { return _gaussHorizontal.getSync(); }

    const CmdSync &draw(
            const std::vector<vk::Semaphore> &waitSemaphores,
            const std::vector<vk::Fence> &waitFences);
private:
    Gauss1D _gaussVertical;
    Gauss1D _gaussHorizontal;
};