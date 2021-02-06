#include "magma/vk/Context.h"

#include "magma/vk/vulkan_common.h"
#include "magma/vk/Extensions.h"
#include "magma/vk/validationLayers/DebugMessenger.h"

#if ( VULKAN_HPP_DISPATCH_LOADER_DYNAMIC == 1 )
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE
#endif

Context::Context() {
    #if ( VULKAN_HPP_DISPATCH_LOADER_DYNAMIC == 1 )
        static vk::DynamicLoader  dl;
        PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr =
                dl.getProcAddress<PFN_vkGetInstanceProcAddr>( "vkGetInstanceProcAddr" );
        VULKAN_HPP_DEFAULT_DISPATCHER.init( vkGetInstanceProcAddr );
    #endif

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

    auto requiredExtensions = Extensions::requiredExtensions();
    Extensions::printRequiredExtensions();
    Extensions::printAvailableExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
    createInfo.ppEnabledExtensionNames = requiredExtensions.data();

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
    _c_instance = VkInstance(_instance);

    #if ( VULKAN_HPP_DISPATCH_LOADER_DYNAMIC == 1 )
        // initialize function pointers for instance
        VULKAN_HPP_DEFAULT_DISPATCHER.init( _instance );
    #endif
}

Context::~Context() {
    _instance.destroy();
}
