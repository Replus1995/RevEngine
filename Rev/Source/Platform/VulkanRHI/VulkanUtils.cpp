#include "VulkanUtils.h"
#include "VulkanContext.h"
#include "VulkanBuffer.h"
#include "Core/VulkanInit.h" 

namespace Rev
{

void FVulkanUtils::TransitionImage(VkCommandBuffer CmdBuffer, VkImage Image, VkImageLayout CurrentLayout, VkImageLayout NextLayout, VkImageAspectFlags AspectMask)
{
	VkImageMemoryBarrier2 ImageBarrier{};
	ImageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
	ImageBarrier.pNext = nullptr;

	ImageBarrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
	ImageBarrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
	ImageBarrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
	ImageBarrier.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;

	ImageBarrier.oldLayout = CurrentLayout;
	ImageBarrier.newLayout = NextLayout;

	VkImageSubresourceRange ImageRange{};
	ImageRange.aspectMask = AspectMask;
	ImageRange.baseMipLevel = 0;
	ImageRange.levelCount = VK_REMAINING_MIP_LEVELS;
	ImageRange.baseArrayLayer = 0;
	ImageRange.layerCount = VK_REMAINING_ARRAY_LAYERS;

	ImageBarrier.subresourceRange = ImageRange;
	ImageBarrier.image = Image;

	VkDependencyInfo DependencyInfo{};
	DependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
	DependencyInfo.pNext = nullptr;
	DependencyInfo.imageMemoryBarrierCount = 1;
	DependencyInfo.pImageMemoryBarriers = &ImageBarrier;

	vkCmdPipelineBarrier2(CmdBuffer, &DependencyInfo);
}

void FVulkanUtils::BlitImage(VkCommandBuffer CmdBuffer, VkImage DstImage, VkExtent3D DstExtent, VkImage SrcImage, VkExtent3D SrcExtent)
{
	VkImageBlit2 BlitRegion{};
	BlitRegion.sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2;
	BlitRegion.pNext = nullptr;

	BlitRegion.srcOffsets[1].x = SrcExtent.width;
	BlitRegion.srcOffsets[1].y = SrcExtent.height;
	BlitRegion.srcOffsets[1].z = SrcExtent.depth;

	BlitRegion.dstOffsets[1].x = DstExtent.width;
	BlitRegion.dstOffsets[1].y = DstExtent.height;
	BlitRegion.dstOffsets[1].z = DstExtent.depth;

	BlitRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	BlitRegion.srcSubresource.baseArrayLayer = 0;
	BlitRegion.srcSubresource.layerCount = 1;
	BlitRegion.srcSubresource.mipLevel = 0;

	BlitRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	BlitRegion.dstSubresource.baseArrayLayer = 0;
	BlitRegion.dstSubresource.layerCount = 1;
	BlitRegion.dstSubresource.mipLevel = 0;

	VkBlitImageInfo2 BlitInfo{};
	BlitInfo.sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2;
	BlitInfo.pNext = nullptr;

	BlitInfo.dstImage = DstImage;
	BlitInfo.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	BlitInfo.srcImage = SrcImage;
	BlitInfo.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
	BlitInfo.filter = VK_FILTER_LINEAR;
	BlitInfo.regionCount = 1;
	BlitInfo.pRegions = &BlitRegion;

	vkCmdBlitImage2(CmdBuffer, &BlitInfo);
}

void FVulkanUtils::ImmediateUploadImage(FVulkanContext* Context, VkImage Image, VkImageAspectFlags AspectMask, VkExtent3D Extent, const void* InData, uint32 InSize, uint8 InMipLevel, uint16 InArrayIndex, uint16 InArrayCount)
{
	FVulkanStageBuffer UploadBuffer(InSize);
	memcpy(UploadBuffer.GetMappedData(), InData, InSize);

	Context->ImmediateSubmit([&](VkCommandBuffer ImmCmdBuffer) {
		TransitionImage(ImmCmdBuffer, Image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, AspectMask);

		VkBufferImageCopy CopyRegion = {};
		CopyRegion.bufferOffset = 0;
		CopyRegion.bufferRowLength = 0;
		CopyRegion.bufferImageHeight = 0;

		CopyRegion.imageSubresource.aspectMask = AspectMask;
		CopyRegion.imageSubresource.mipLevel = InMipLevel;
		CopyRegion.imageSubresource.baseArrayLayer = InArrayIndex;
		CopyRegion.imageSubresource.layerCount = InArrayCount;
		CopyRegion.imageExtent = Extent;

		// copy the buffer into the image
		vkCmdCopyBufferToImage(ImmCmdBuffer, UploadBuffer.Buffer, Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &CopyRegion);

		TransitionImage(ImmCmdBuffer, Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, AspectMask);
	});
}

void FVulkanUtils::ImmediateClearImage(FVulkanContext* Context, VkImage Image, VkImageAspectFlags AspectMask, VkClearValue InClearValue, uint8 InMipLevel, uint8 InMipCount, uint16 InArrayIndex, uint16 InArrayCount)
{
	bool bColorImage = AspectMask & VK_IMAGE_ASPECT_COLOR_BIT;
	Context->ImmediateSubmit([&](VkCommandBuffer ImmCmdBuffer) {
		TransitionImage(ImmCmdBuffer, Image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL, AspectMask);

		VkImageSubresourceRange Range{};
		Range.aspectMask = AspectMask;
		Range.baseMipLevel = InMipLevel;
		Range.levelCount = InMipCount;
		Range.baseArrayLayer = InArrayIndex;
		Range.layerCount = InArrayCount;

		if (bColorImage)
		{
			vkCmdClearColorImage(ImmCmdBuffer, Image, VK_IMAGE_LAYOUT_GENERAL, &InClearValue.color, 1, &Range);
		}
		else
		{
			vkCmdClearDepthStencilImage(ImmCmdBuffer, Image, VK_IMAGE_LAYOUT_GENERAL, &InClearValue.depthStencil, 1, &Range);
		}

		TransitionImage(ImmCmdBuffer, Image, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, AspectMask);
	});
}

void FVulkanUtils::ImmediateUploadBuffer(FVulkanContext* Context, VkBuffer DstBuffer, const void* InData, uint32 InSize, uint32 InOffset)
{
	FVulkanStageBuffer UploadBuffer(InSize);
	memcpy(UploadBuffer.GetMappedData(), InData, InSize);

	Context->ImmediateSubmit([&](VkCommandBuffer ImmCmdBuffer) {
		VkBufferCopy CopyInfo{};
		CopyInfo.dstOffset = InOffset;
		CopyInfo.srcOffset = 0;
		CopyInfo.size = InSize;
		vkCmdCopyBuffer(ImmCmdBuffer, UploadBuffer.Buffer, DstBuffer, 1, &CopyInfo);
	});
}

}