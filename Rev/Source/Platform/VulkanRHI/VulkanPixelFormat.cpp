#include "VulkanPixelFormat.h"
#include "Rev/Core/Assert.h"

namespace Rev
{

constexpr VkFormat sVkFormatMappings[] = {
	VK_FORMAT_UNDEFINED,
	VK_FORMAT_R8_UNORM,
	VK_FORMAT_R8_UINT,
	VK_FORMAT_R8_SINT,
	VK_FORMAT_R16_UNORM,
	VK_FORMAT_R16_UINT,
	VK_FORMAT_R16_SINT,
	VK_FORMAT_R16_SFLOAT,
	VK_FORMAT_R32_UINT,
	VK_FORMAT_R32_SINT,
	VK_FORMAT_R32_SFLOAT,
	VK_FORMAT_R8G8_UNORM,
	VK_FORMAT_R16G16_UNORM,
	VK_FORMAT_R16G16_SFLOAT,
	VK_FORMAT_R32G32_SFLOAT,


	{ PF_RGBA32F, { VK_FORMAT_R32G32B32A32_SFLOAT,	4, 4 } },
	{ PF_RGBA8,		{ VK_FORMAT_R8G8B8A8_UNORM,			4, 1 } },
	{ PF_R8,			{ VK_FORMAT_R8_UNORM,				1, 1 } },
	{ PF_R16,			{ VK_FORMAT_R16_UNORM,				1, 2 } },
	{ PF_RGB8,			{ VK_FORMAT_R8G8B8_UNORM,			3, 1 } },
	{ PF_DepthStencil,	{ VK_FORMAT_D24_UNORM_S8_UINT,		1, 4 } },
	{ PF_ShadowDepth,	{ VK_FORMAT_D32_SFLOAT,				4, 1 } },
};


void FVkPixelFormat::InitPlatformFormats()
{
}

}


