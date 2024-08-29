#include "VkCore.h"
#include "Rev/Core/Assert.h"
#include "Rev/Render/RenderCmd.h"

namespace Rev
{

VkInstance FVkCore::GetInstance()
{
	return GetContext()->GetInstance().GetInstance();
}

VkPhysicalDevice FVkCore::GetPhysicalDevice()
{
	return GetContext()->GetDevice().GetPhysicalDevice();
}

VkDevice FVkCore::GetDevice()
{
	return GetContext()->GetDevice().GetLogicalDevice();
}

VkQueue FVkCore::GetQueue(EVkQueueKind InKind)
{
	return GetContext()->GetDevice().GetQueue(InKind);
}

uint32 FVkCore::GetQueueFamily(EVkQueueKind InKind)
{
	return GetContext()->GetDevice().GetQueueFamily(InKind);
}

VmaAllocator FVkCore::GetAllocator()
{
	return GetContext()->GetAllocator();
}

VkCommandBuffer FVkCore::GetMainCmdBuffer()
{
	return GetContext()->GetMainCmdBuffer();
}

void FVkCore::ImmediateSubmit(std::function<void(VkCommandBuffer)>&& Func)
{
	GetContext()->ImmediateSubmit(std::move(Func));
}

FVkContext* FVkCore::GetContext()
{
	REV_CORE_ASSERT(GetRenderAPI() == ERenderAPI::Vulkan);
	return static_cast<FVkContext*>(RenderCmd::GetContext());
}


}