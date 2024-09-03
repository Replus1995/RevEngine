#pragma once
#include "Rev/Render/PixelFormat.h"
#include <vulkan/vulkan.h>

namespace Rev
{

struct FVkFormatInfo
{
	VkFormat Format;
	VkImageAspectFlags AspectFlags;
	uint16 Channels;
	uint16 PixelDepth;
};

class FVkPixelFormat
{
public:
	static FVkFormatInfo TranslatePixelFormat(EPixelFormat InFormat, bool bSRGB); //packed?
};

}