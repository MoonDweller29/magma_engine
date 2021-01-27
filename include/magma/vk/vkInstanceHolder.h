#pragma once
#include <vulkan/vulkan.h>

class VkInstanceHolder
{
private:
	VkInstance instance;
public:
	const VkInstance &get(){ return instance; }

	VkInstanceHolder();
	~VkInstanceHolder();
};