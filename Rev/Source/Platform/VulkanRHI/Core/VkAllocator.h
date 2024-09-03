#pragma once
#include "Rev/Core/Base.h"
#include <Vulkan/vulkan.h>

namespace Rev
{

class FVulkanAllocator
{
public:
	static VkAllocationCallbacks* Callbacks;
};

}