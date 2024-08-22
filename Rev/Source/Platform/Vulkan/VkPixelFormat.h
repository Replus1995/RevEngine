#pragma once
#include "Rev/Render/PixelFormat.h"
#include <vulkan/vulkan.h>

namespace Rev
{

struct FVkFormatInfo
{
	VkFormat Format;
	VkImageAspectFlags AspectFlags;
	//GLenum DataType;
	uint32 PixelSize;
};

class FVkPixelFormat
{
public:
	static FVkFormatInfo TranslatePixelFormat(EPixelFormat InFormat, bool bSRGB); //packed?
};

}