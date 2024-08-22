#pragma once
#include "Rev/Core/Base.h"
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>
#include "VkDefines.h"


namespace Rev
{

class FVkCore
{
public:
	static VkInstance GetInstance();
	static VkDevice GetDevice();
	static VmaAllocator GetAllocator();

};

}