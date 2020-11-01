#include "vulkan_common.h"
#include <vector>

void print_available_extensions();
std::vector<const char*> get_required_extensions();
void print_required_extensions(const std::vector<const char*> &extensions);