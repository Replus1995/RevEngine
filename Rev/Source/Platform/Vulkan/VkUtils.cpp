#include "VkUtils.h"

namespace Rev
{

void FVkUtils::TransitionImage(VkCommandBuffer CmdBuffer, VkImage Image, VkImageLayout CurrentLayout, VkImageLayout NextLayout)
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

    VkImageAspectFlags AspectMask = (NextLayout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
    ImageBarrier.subresourceRange = FVkInit::ImageSubresourceRange(AspectMask);
    ImageBarrier.image = Image;

    VkDependencyInfo DependencyInfo{};
    DependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    DependencyInfo.pNext = nullptr;
    DependencyInfo.imageMemoryBarrierCount = 1;
    DependencyInfo.pImageMemoryBarriers = &ImageBarrier;

    vkCmdPipelineBarrier2(CmdBuffer, &DependencyInfo);
}

void FVkUtils::BlitImage(VkCommandBuffer CmdBuffer, VkImage SrcImage, VkImage DstImage, VkExtent2D SrcExtent, VkExtent2D DstExtent)
{
	VkImageBlit2 BlitRegion{};
	BlitRegion.sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2;
	BlitRegion.pNext = nullptr;

	BlitRegion.srcOffsets[1].x = SrcExtent.width;
	BlitRegion.srcOffsets[1].y = SrcExtent.height;
	BlitRegion.srcOffsets[1].z = 1;

	BlitRegion.dstOffsets[1].x = DstExtent.width;
	BlitRegion.dstOffsets[1].y = DstExtent.height;
	BlitRegion.dstOffsets[1].z = 1;

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


}


