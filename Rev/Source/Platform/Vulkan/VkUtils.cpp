#include "VkUtils.h"

namespace Rev
{

VkCommandPoolCreateInfo FVkInit::CmdPoolCreateInfo(uint32 InQueueFamily, VkCommandPoolCreateFlags InFlags)
{
    VkCommandPoolCreateInfo CreateInfo = {};
    CreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    CreateInfo.pNext = nullptr;
    CreateInfo.queueFamilyIndex = InQueueFamily;
    CreateInfo.flags = InFlags;
    return CreateInfo;
}

VkCommandBufferAllocateInfo FVkInit::CmdBufferAllocateInfo(VkCommandPool InPool, uint32 InCount)
{
    VkCommandBufferAllocateInfo CreateInfo{};
    CreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    CreateInfo.pNext = nullptr;
    CreateInfo.commandPool = InPool;
    CreateInfo.commandBufferCount = InCount;
    CreateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    return CreateInfo;
}

VkCommandBufferBeginInfo FVkInit::CmdBufferBeginInfo(VkCommandBufferUsageFlags InFlags)
{
    VkCommandBufferBeginInfo CreateInfo{};
    CreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    CreateInfo.pNext = nullptr;
    CreateInfo.pInheritanceInfo = nullptr;
    CreateInfo.flags = InFlags;
    return CreateInfo;
}

VkFenceCreateInfo FVkInit::FenceCreateInfo(VkFenceCreateFlags InFlags)
{
    VkFenceCreateInfo CreateInfo{};
    CreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    CreateInfo.pNext = nullptr;
    CreateInfo.flags = InFlags;
    return CreateInfo;
}

VkSemaphoreCreateInfo FVkInit::SemaphoreCreateInfo(VkSemaphoreCreateFlags InFlags)
{
    VkSemaphoreCreateInfo CreateInfo{};
    CreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    CreateInfo.pNext = nullptr;
    CreateInfo.flags = InFlags;
    return CreateInfo;
}

VkImageSubresourceRange FVkInit::ImageSubresourceRange(VkImageAspectFlags InAspectMask)
{
    VkImageSubresourceRange Range{};
    Range.aspectMask = InAspectMask;
    Range.baseMipLevel = 0;
    Range.levelCount = VK_REMAINING_MIP_LEVELS;
    Range.baseArrayLayer = 0;
    Range.layerCount = VK_REMAINING_ARRAY_LAYERS;

    return Range;
}

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


