#pragma once
#include "Rev/Core/Base.h"
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>
#include "VkDefines.h"
#include "VkInitializer.h"


namespace Rev
{

class FVkUtils
{
public:
	static VkDevice GetDevice();
	static VmaAllocator GetAllocator();

	static void TransitionImage(VkCommandBuffer CmdBuffer, VkImage Image, VkImageLayout CurrentLayout, VkImageLayout NextLayout);

};

}