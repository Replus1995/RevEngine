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
	VK_FORMAT_R8G8B8_UNORM,
	VK_FORMAT_R16G16B16_UNORM,
	VK_FORMAT_R16G16B16_SFLOAT,
	VK_FORMAT_R32G32B32_UINT,
	VK_FORMAT_R32G32B32_SINT,
	VK_FORMAT_R32G32B32_SFLOAT,
	VK_FORMAT_R8G8B8A8_UNORM,
	VK_FORMAT_R8G8B8A8_SNORM,
	VK_FORMAT_R8G8B8A8_UINT,
	VK_FORMAT_R8G8B8A8_SINT,
	VK_FORMAT_R16G16B16A16_UNORM,
	VK_FORMAT_R16G16B16A16_SNORM,
	VK_FORMAT_R16G16B16A16_UINT,
	VK_FORMAT_R16G16B16A16_SINT,
	VK_FORMAT_R16G16B16A16_SFLOAT,
	VK_FORMAT_R32G32B32A32_UINT,
	VK_FORMAT_R32G32B32A32_SINT,
	VK_FORMAT_R32G32B32A32_SFLOAT,

	VK_FORMAT_D24_UNORM_S8_UINT,
	VK_FORMAT_D32_SFLOAT,

	VK_FORMAT_R8_SNORM,
	VK_FORMAT_R16_SNORM,
	VK_FORMAT_R8G8_SNORM,
	VK_FORMAT_R8G8_SINT,
	VK_FORMAT_R8G8_UINT,
	VK_FORMAT_R16G16_SNORM,
	VK_FORMAT_R16G16_SINT,
	VK_FORMAT_R16G16_UINT,
	VK_FORMAT_R32G32_SINT,
	VK_FORMAT_R32G32_UINT,

	VK_FORMAT_UNDEFINED,
	VK_FORMAT_UNDEFINED,
};


void FVulkanPixelFormat::InitPlatformFormats()
{
	static_assert(sizeof(sVkFormatMappings) / sizeof(VkFormat) == PF_Count);

	for (uint16 i = 0; i < PF_Count; i++)
	{
		GPixelFormats[i].PlatformFormat = sVkFormatMappings[i];
		if (i >= PF_DepthStencil)
		{
			GPixelFormats[i].Supported = sVkFormatMappings[i] != VK_FORMAT_UNDEFINED;
		}
	}

}

}


