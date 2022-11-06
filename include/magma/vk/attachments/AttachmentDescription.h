#pragma once

#include <vulkan/vulkan.hpp>

class AttachmentDescription {
public:
    inline explicit AttachmentDescription(vk::Format format) {
        _attachment.format         = format;
        _attachment.samples        = vk::SampleCountFlagBits::e1; //for multisampling
        _attachment.loadOp         = vk::AttachmentLoadOp::eDontCare;
        _attachment.storeOp        = vk::AttachmentStoreOp::eStore;
        _attachment.stencilLoadOp  = vk::AttachmentLoadOp::eDontCare;
        _attachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
        _attachment.initialLayout  = vk::ImageLayout::eUndefined;
        _attachment.finalLayout    = vk::ImageLayout::eShaderReadOnlyOptimal;
    }

    inline AttachmentDescription &setSampleCount(vk::SampleCountFlagBits sampleCountFlagBits) {
        _attachment.samples = sampleCountFlagBits;
        return *this;
    }

    inline AttachmentDescription &setLoadStoreOp(vk::AttachmentLoadOp loadOp, vk::AttachmentStoreOp storeOp) {
        _attachment.loadOp   = loadOp;
        _attachment.storeOp  = storeOp;
        return *this;
    }

    inline AttachmentDescription &setStencilLoadStoreOp(vk::AttachmentLoadOp loadOp, vk::AttachmentStoreOp storeOp) {
        _attachment.stencilLoadOp  = loadOp;
        _attachment.stencilStoreOp = storeOp;
        return *this;
    }

    inline AttachmentDescription &setLayouts(vk::ImageLayout initialLayout, vk::ImageLayout finalLayout) {
        _attachment.initialLayout  = initialLayout;
        _attachment.finalLayout    = finalLayout;
        return *this;
    }

    operator vk::AttachmentDescription() { return _attachment; }
private:
    vk::AttachmentDescription _attachment;
};
