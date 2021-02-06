#include "magma/vk/Context.h"

#include "magma/vk/vulkan_common.h"
#include "magma/vk/vk_extensions.h"
#include "magma/vk/validationLayers.h"

Context::Context() {
    if (ValidationLayers::ENABLED && !ValidationLayers::supported()) {
        LOG_WARNING("validation layers requested, but not available!");
    }

    vk::ApplicationInfo appInfo(
        "magma-editor",
        VK_MAKE_VERSION(1, 0, 0),
        "magma engine",
        VK_MAKE_VERSION(1, 0, 0),
        VK_API_VERSION_1_0
    );

    vk::InstanceCreateInfo createInfo;
    createInfo.pApplicationInfo = &appInfo;

    auto requiredExtensions = get_required_extensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
    createInfo.ppEnabledExtensionNames = requiredExtensions.data();
    print_required_extensions(requiredExtensions);

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
    if (ValidationLayers::ENABLED) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(ValidationLayers::validationLayers.size());
        createInfo.ppEnabledLayerNames = ValidationLayers::validationLayers.data();
        DebugMessenger::fillCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
    } else {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
    }

    auto [result, _instance] = vk::createInstance(createInfo);
    VK_HPP_CHECK_ERR(result, "failed to create instance!");
    print_available_extensions();
}

Context::~Context() {
    _instance.destroy();
}
