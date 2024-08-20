#pragma once
#include "Rev/Render/PixelFormat.h"
#include <vulkan/vulkan.h>

namespace Rev
{

struct FVkFormatData
{
	VkFormat Format;
	VkImageAspectFlags AspectFlags;
	//GLenum DataType;
	uint32 PixelSize;
};

class FVkPixelFormat
{
public:
	static FVkFormatData TranslatePixelFormat(EPixelFormat InFormat, bool bSRGB); //packed?
};

}