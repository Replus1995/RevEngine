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


}


