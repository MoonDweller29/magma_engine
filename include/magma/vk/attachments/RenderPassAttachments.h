#pragma once

#include <vector>
#include "magma/vk/attachments/AttachmentDescription.h"

class RenderPassAttachments {
public:
    RenderPassAttachments(const std::vector<vk::AttachmentDescription> &colorAttachments) {
        for (auto & description: colorAttachments) {
            addColorAttachment(description);
        }
    }

    RenderPassAttachments(const std::vector<vk::AttachmentDescription> &colorAttachments, vk::AttachmentDescription depthAttachment) {
        for (auto & description: colorAttachments) {
            addColorAttachment(description);
        }
        addDepthAttachment(depthAttachment);
    }

    inline const std::vector<vk::AttachmentDescription>   &getDescriptions()          { return _descriptions;        }
    inline const std::vector<vk::AttachmentReference>     &getColorAttachmentRefs()   { return _colorAttachmentRefs; }
    inline const vk::AttachmentReference                  &getDepthAttachmentRef()    { return _depthAttachmentRef;  }

private:
    std::vector<vk::AttachmentDescription>   _descriptions;
    std::vector<vk::AttachmentReference>     _colorAttachmentRefs;
    vk::AttachmentReference                  _depthAttachmentRef;

    inline void addColorAttachment(vk::AttachmentDescription description) {
        uint32_t index = _descriptions.size();
        _descriptions.push_back(description);
        _colorAttachmentRefs.emplace_back(index, vk::ImageLayout::eColorAttachmentOptimal);
    }
    inline void addDepthAttachment(vk::AttachmentDescription description) {
        uint32_t index = _descriptions.size();
        _descriptions.push_back(description);
        _depthAttachmentRef = vk::AttachmentReference(index, vk::ImageLayout::eDepthStencilAttachmentOptimal);
    }
};