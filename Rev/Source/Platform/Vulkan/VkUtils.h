#pragma once
#include "Rev/Core/Base.h"
#include <vulkan/vulkan.h>
#include "VkDefines.h"
#include "VkInitializer.h"


namespace Rev
{

class FVkUtils
{
public:
	static void TransitionImage(VkCommandBuffer CmdBuffer, VkImage Image, VkImageLayout CurrentLayout, VkImageLayout NextLayout);

};

}