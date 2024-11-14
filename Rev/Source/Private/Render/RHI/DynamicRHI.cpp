#include "Rev/Render/RHI/DynamicRHI.h"
#include "Rev/Core/Assert.h"

#include "VulkanRHI/VulkanDynamicRHI.h"

namespace Rev
{
IDyanmicRHI* GDynamicRHI = nullptr;

void RHIInit(ERenderAPI InAPI)
{
	switch (InAPI)
	{
	case Rev::ERenderAPI::Vulkan:
		GDynamicRHI = new FVulkanDynamicRHI();
		break;
	default:
		REV_CORE_ASSERT(false)
		break;
	}

	GDynamicRHI->Init();
}

void RHIExit()
{
	GDynamicRHI->Cleanup();
	delete GDynamicRHI;
	GDynamicRHI = nullptr;
}

}