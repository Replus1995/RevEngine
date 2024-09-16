#include "VulkanPixelFormat.h"
#include "Rev/Core/Assert.h"

namespace Rev
{

struct FVkFormatMapping
{
	EPixelFormat RevFormat;
	FVulkanFormatInfo FormatInfo;
};

constexpr FVkFormatMapping sVkFormatMappings[] = {
	{ PF_Unknown,		{ VK_FORMAT_UNDEFINED,				0, 0 } },
	{ PF_R32G32B32A32F, { VK_FORMAT_R32G32B32A32_SFLOAT,	4, 4 } },
	{ PF_R8G8B8A8,		{ VK_FORMAT_R8G8B8A8_UNORM,			4, 1 } },
	{ PF_R8,			{ VK_FORMAT_R8_UNORM,				1, 1 } },
	{ PF_R16,			{ VK_FORMAT_R16_UNORM,				1, 2 } },
	{ PF_RGB8,			{ VK_FORMAT_R8G8B8_UNORM,			3, 1 } },
	{ PF_DepthStencil,	{ VK_FORMAT_D24_UNORM_S8_UINT,		1, 4 } },
	{ PF_ShadowDepth,	{ VK_FORMAT_D32_SFLOAT,				4, 1 } },
};


FVulkanFormatInfo FVkPixelFormat::TranslatePixelFormat(EPixelFormat InFormat, bool bSRGB)
{
	REV_CORE_ASSERT(InFormat != EPixelFormat::PF_Unknown);
	if (!bSRGB)
	{
		return sVkFormatMappings[InFormat].FormatInfo;
	}
	else
	{
		/*switch (InFormat)
		{
		case Rev::PF_R8G8B8A8:
			return { GL_SRGB8_ALPHA8, GL_RGBA, GL_UNSIGNED_BYTE, 4 };
		case Rev::PF_RGB8:
			return { GL_SRGB8, GL_RGB, GL_UNSIGNED_BYTE, 3 };
		default:
			break;
		}
		REV_CORE_ASSERT(false, "Unknown SRGB format");*/
		return { VK_FORMAT_UNDEFINED, 0, 0 };
	}
}

}


