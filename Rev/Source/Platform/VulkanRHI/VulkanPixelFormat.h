#pragma once
#include "Rev/Render/PixelFormat.h"
#include <vulkan/vulkan.h>

namespace Rev
{
class FVkPixelFormat
{
public:
	static void InitPlatformFormats();
};

}