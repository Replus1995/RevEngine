#pragma once
#include "Rev/Core/Base.h"
#include <vulkan/vulkan.h>
#include <vector>

namespace Rev
{
class FVulkanContext;
class FVulkanUtils
{
public:
	static void TransitionImage(VkCommandBuffer CmdBuffer, VkImage Image, VkImageLayout CurrentLayout, VkImageLayout NextLayout, VkImageAspectFlags AspectMask, uint32 BaseMip = 0, uint32 NumMips = VK_REMAINING_MIP_LEVELS, uint32 BaseLayer = 0, uint32 NumLayers = VK_REMAINING_ARRAY_LAYERS);
	static void BlitImage(VkCommandBuffer CmdBuffer, VkImage DstImage, VkExtent3D DstExtent, VkImage SrcImage, VkExtent3D SrcExtent);

	static void ImmediateUploadImage(FVulkanContext* Context, VkImage Image, VkImageAspectFlags AspectMask, VkExtent3D Extent, const void* InData, uint32 InSize, uint8 InMipLevel, uint16 InArrayIndex = 0, uint16 InArrayCount = 1, VkImageLayout OldLayout = VK_IMAGE_LAYOUT_UNDEFINED, VkOffset3D Offset = { 0, 0, 0 }, uint32 BufferRowLength = 0, uint32 BufferImageHeight = 0);
	static void ImmediateClearImage(FVulkanContext* Context, VkImage Image, VkImageAspectFlags AspectMask, VkClearValue InClearValue, uint8 InMipLevel, uint8 InMipCount, uint16 InArrayIndex = 0, uint16 InArrayCount = 1, VkImageLayout OldLayout = VK_IMAGE_LAYOUT_UNDEFINED);
	static void ImmediateUploadBuffer(FVulkanContext* Context, VkBuffer DstBuffer, const void* InData, uint32 InSize, uint32 InOffset);
};


}
