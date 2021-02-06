#pragma once
#include <vulkan/vulkan.h>

class Context
{
private:
	VkInstance instance;
public:
	const VkInstance &get(){ return instance; }

	Context();
	~Context();
};