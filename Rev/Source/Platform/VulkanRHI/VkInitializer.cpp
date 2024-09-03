#include "VkInitializer.h"

namespace Rev
{

VkCommandPoolCreateInfo FVulkanInit::CmdPoolCreateInfo(uint32 InQueueFamily, VkCommandPoolCreateFlags InFlags)
{
    VkCommandPoolCreateInfo CreateInfo = {};
    CreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    CreateInfo.pNext = nullptr;
    CreateInfo.queueFamilyIndex = InQueueFamily;
    CreateInfo.flags = InFlags;
    return CreateInfo;
}

VkCommandBufferAllocateInfo FVulkanInit::CmdBufferAllocateInfo(VkCommandPool InPool, uint32 InCount)
{
    VkCommandBufferAllocateInfo CreateInfo{};
    CreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    CreateInfo.pNext = nullptr;
    CreateInfo.commandPool = InPool;
    CreateInfo.commandBufferCount = InCount;
    CreateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    return CreateInfo;
}

VkCommandBufferBeginInfo FVulkanInit::CmdBufferBeginInfo(VkCommandBufferUsageFlags InFlags)
{
    VkCommandBufferBeginInfo CreateInfo{};
    CreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    CreateInfo.pNext = nullptr;
    CreateInfo.pInheritanceInfo = nullptr;
    CreateInfo.flags = InFlags;
    return CreateInfo;
}

VkCommandBufferSubmitInfo FVulkanInit::CmdBufferSubmitInfo(VkCommandBuffer InCmdBuffer)
{
    VkCommandBufferSubmitInfo SubmitInfo{};
    SubmitInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
    SubmitInfo.pNext = nullptr;
    SubmitInfo.commandBuffer = InCmdBuffer;
    SubmitInfo.deviceMask = 0;
    return SubmitInfo;
}

VkFenceCreateInfo FVulkanInit::FenceCreateInfo(VkFenceCreateFlags InFlags)
{
    VkFenceCreateInfo CreateInfo{};
    CreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    CreateInfo.pNext = nullptr;
    CreateInfo.flags = InFlags;
    return CreateInfo;
}

VkSemaphoreCreateInfo FVulkanInit::SemaphoreCreateInfo(VkSemaphoreCreateFlags InFlags)
{
    VkSemaphoreCreateInfo CreateInfo{};
    CreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    CreateInfo.pNext = nullptr;
    CreateInfo.flags = InFlags;
    return CreateInfo;
}

VkSemaphoreSubmitInfo FVulkanInit::SemaphoreSubmitInfo(VkPipelineStageFlags2 InStageMask, VkSemaphore InSemaphore)
{
    VkSemaphoreSubmitInfo SubmitInfo{};
    SubmitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
    SubmitInfo.pNext = nullptr;
    SubmitInfo.semaphore = InSemaphore;
    SubmitInfo.stageMask = InStageMask;
    SubmitInfo.deviceIndex = 0;
    SubmitInfo.value = 1;
    return SubmitInfo;
}

VkSubmitInfo2 FVulkanInit::SubmitInfo(VkCommandBufferSubmitInfo* CmdBufferInfo, VkSemaphoreSubmitInfo* SignalSemaphoreInfo, VkSemaphoreSubmitInfo* WaitSemaphoreInfo)
{
    VkSubmitInfo2 SubmitInfo{};
    SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
    SubmitInfo.pNext = nullptr;

    SubmitInfo.waitSemaphoreInfoCount = WaitSemaphoreInfo == nullptr ? 0 : 1;
    SubmitInfo.pWaitSemaphoreInfos = WaitSemaphoreInfo;

    SubmitInfo.signalSemaphoreInfoCount = SignalSemaphoreInfo == nullptr ? 0 : 1;
    SubmitInfo.pSignalSemaphoreInfos = SignalSemaphoreInfo;

    SubmitInfo.commandBufferInfoCount = 1;
    SubmitInfo.pCommandBufferInfos = CmdBufferInfo;

    return SubmitInfo;
}

VkImageCreateInfo FVulkanInit::ImageCreateInfo2D(VkFormat Format, VkImageUsageFlags UsageFlags, VkExtent3D Extent)
{
    VkImageCreateInfo CreateInfo{};
    CreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    CreateInfo.pNext = nullptr;

    CreateInfo.imageType = VK_IMAGE_TYPE_2D;

    CreateInfo.format = Format;
    CreateInfo.extent = Extent;

    CreateInfo.mipLevels = 1;
    CreateInfo.arrayLayers = 1;

    //for MSAA. we will not be using it by default, so default it to 1 sample per pixel.
    CreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;

    //optimal tiling, which means the image is stored on the best gpu format
    CreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    CreateInfo.usage = UsageFlags;

    return CreateInfo;
}

VkImageViewCreateInfo FVulkanInit::ImageViewCreateInfo2D(VkFormat Format, VkImage Image, VkImageAspectFlags AspectFlags)
{
    VkImageViewCreateInfo CreateInfo{};
    CreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    CreateInfo.pNext = nullptr;

    CreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    CreateInfo.image = Image;
    CreateInfo.format = Format;
    CreateInfo.subresourceRange.baseMipLevel = 0;
    CreateInfo.subresourceRange.levelCount = 1;
    CreateInfo.subresourceRange.baseArrayLayer = 0;
    CreateInfo.subresourceRange.layerCount = 1;
    CreateInfo.subresourceRange.aspectMask = AspectFlags;
    return CreateInfo;
}

VkImageSubresourceRange FVulkanInit::ImageSubresourceRange(VkImageAspectFlags InAspectMask)
{
    VkImageSubresourceRange Range{};
    Range.aspectMask = InAspectMask;
    Range.baseMipLevel = 0;
    Range.levelCount = VK_REMAINING_MIP_LEVELS;
    Range.baseArrayLayer = 0;
    Range.layerCount = VK_REMAINING_ARRAY_LAYERS;

    return Range;
}

VkRenderingAttachmentInfo FVulkanInit::AttachmentInfo(VkImageView ImageView, VkClearValue* ClearValue, VkImageLayout ImageLayout)
{
    VkRenderingAttachmentInfo ColorAttachment{};
    ColorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    ColorAttachment.pNext = nullptr;

    ColorAttachment.imageView = ImageView;
    ColorAttachment.imageLayout = ImageLayout;
    ColorAttachment.loadOp = ClearValue ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
    ColorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    if (ClearValue) {
        ColorAttachment.clearValue = *ClearValue;
    }

    return ColorAttachment;
}

}