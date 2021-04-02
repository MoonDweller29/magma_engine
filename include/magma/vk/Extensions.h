#include <vector>
#include "magma/vk/vulkan_common.h"

class Extensions {
public:
    Extensions() = delete;

    static std::vector<const char*> requiredExtensions();
    static void printAvailableExtensions();
    static void printRequiredExtensions();
};
