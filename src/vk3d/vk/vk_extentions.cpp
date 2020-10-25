#include "vk3d/vk/vk_extentions.h"
#include <vector>
#include <iostream>
#include "vk3d/vk/validationLayers.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

void print_available_extensions()
{
	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> extensions(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

	std::cout << "available extensions:\n";
	for (const auto& extension : extensions)
	{
	    std::cout << '\t' << extension.extensionName << '\n';
	}
}

std::vector<const char*> get_required_extensions()
{
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (enableValidationLayers) {
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}

void print_required_extensions(const std::vector<const char*> &extensions)
{
    uint32_t  count = extensions.size();
    std::cout <<"Required extensions count = "<< count << std::endl;
    for (uint32_t i = 0; i < count; ++i)
    {
        std::cout << i+1 << ": " << extensions[i] << std::endl;
    }
}