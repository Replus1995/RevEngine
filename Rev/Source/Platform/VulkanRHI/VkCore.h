#pragma once
#include "VkContext.h"

namespace Rev
{

class FVkContext;
class FVkCore
{
public:
	static VkInstance GetInstance();
	static VkPhysicalDevice GetPhysicalDevice();
	static VkDevice GetDevice();
	static VkQueue GetQueue(EVkQueueKind InKind);
	static uint32 GetQueueFamily(EVkQueueKind InKind);
	static VmaAllocator GetAllocator();
	static VkCommandBuffer GetMainCmdBuffer();

	static void ImmediateSubmit(std::function<void(VkCommandBuffer)>&& Func);

private:
	static FVkContext* GetContext();
};

}


