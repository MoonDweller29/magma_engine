#include "magma/vk/Extensions.h"

#include <vector>
#include <iostream>
#include "magma/vk/validationLayers.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

void Extensions::printAvailableExtensions() {
	auto [result, extensions] = vk::enumerateInstanceExtensionProperties(nullptr);

	std::cout << "available extensions:\n";
	for (const auto& extension : extensions) {
	    std::cout << '\t' << extension.extensionName << '\n';
	}
}

std::vector<const char*> Extensions::requiredExtensions() {
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (ValidationLayers::ENABLED) {
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}

void Extensions::printRequiredExtensions() {
    auto extensions = requiredExtensions();
    uint32_t count = extensions.size();
    std::cout <<"Required extensions count = "<< count << std::endl;
    for (uint32_t i = 0; i < count; ++i) {
        std::cout << i+1 << ": " << extensions[i] << std::endl;
    }
}
