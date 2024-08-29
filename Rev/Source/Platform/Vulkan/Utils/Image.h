#pragma once
#include "Rev/Core/Base.h"
#include <vulkan/vulkan.h>
#include <vector>

namespace Rev::VkUtils
{
void TransitionImage(VkCommandBuffer CmdBuffer, VkImage Image, VkImageLayout CurrentLayout, VkImageLayout NextLayout);
void BlitImage(VkCommandBuffer CmdBuffer, VkImage SrcImage, VkImage DstImage, VkExtent2D SrcExtent, VkExtent2D DstExtent);
void ImmediateUploadImage(VkImage Image, VkImageAspectFlags AspectMask, VkExtent3D Extent, const void* InData, uint32 InSize, uint8 InMipLevel, uint16 InArrayIndex, uint16 InArrayCount);
void ImmediateClearImage(VkImage Image, VkImageAspectFlags AspectMask, VkClearValue InClearValue, uint8 InMipLevel, uint8 InMipCount, uint16 InArrayIndex, uint16 InArrayCount);
}