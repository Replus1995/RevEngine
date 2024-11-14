#include "VulkanInit.h"

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

}