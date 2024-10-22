#pragma once
#include "Rev/Render/PixelFormat.h"
#include <vulkan/vulkan.h>

namespace Rev
{
class FVulkanPixelFormat
{
public:
	static void InitPlatformFormats();
	static VkFormat GetPlatformFormatSRGB(VkFormat InFormat);
};

}