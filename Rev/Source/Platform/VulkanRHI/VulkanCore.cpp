#include "VulkanCore.h"
#include "Rev/Core/Assert.h"
#include "Rev/Render/RenderCmd.h"

namespace Rev
{

VkInstance FVulkanCore::GetInstance()
{
	return GetContext()->GetInstance().GetInstance();
}

VkPhysicalDevice FVulkanCore::GetPhysicalDevice()
{
	return GetContext()->GetDevice().GetPhysicalDevice();
}

VkDevice FVulkanCore::GetDevice()
{
	return GetContext()->GetDevice().GetDevice();
}

VkQueue FVulkanCore::GetQueue(EVulkanQueueKind InKind)
{
	return GetContext()->GetDevice().GetQueue(InKind);
}

uint32 FVulkanCore::GetQueueFamily(EVulkanQueueKind InKind)
{
	return GetContext()->GetDevice().GetQueueFamily(InKind);
}

VmaAllocator FVulkanCore::GetAllocator()
{
	return GetContext()->GetAllocator();
}

VkCommandBuffer FVulkanCore::GetMainCmdBuffer()
{
	return GetContext()->GetMainCmdBuffer();
}

void FVulkanCore::ImmediateSubmit(std::function<void(VkCommandBuffer)>&& Func)
{
	GetContext()->ImmediateSubmit(std::move(Func));
}

FVulkanContext* FVulkanCore::GetContext()
{
	REV_CORE_ASSERT(GetRenderAPI() == ERenderAPI::Vulkan);
	return static_cast<FVulkanContext*>(RenderCmd::GetContext());
}


}