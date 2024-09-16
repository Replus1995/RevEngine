#pragma once
#include "Rev/Render/PixelFormat.h"
#include <vulkan/vulkan.h>

namespace Rev
{

struct FVulkanFormatInfo
{
	VkFormat Format;
	uint16 Channels;
	uint16 PixelDepth;
};

class FVkPixelFormat
{
public:
	static FVulkanFormatInfo TranslatePixelFormat(EPixelFormat InFormat, bool bSRGB); //packed?
};

}