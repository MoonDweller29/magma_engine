#include "magma/vk/vulkan_common.h"
#include <vector>

class Extensions {
public:
    static std::vector<const char*> requiredExtensions();
    static void printAvailableExtensions();
    static void printRequiredExtensions();
};
