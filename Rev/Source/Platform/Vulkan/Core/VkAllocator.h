#pragma once
#include "Rev/Core/Base.h"
#include <Vulkan/vulkan.h>

namespace Rev
{

class FVkAllocator
{
public:
	static VkAllocationCallbacks* Callbacks;
};

}