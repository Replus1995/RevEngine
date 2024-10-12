#include "VulkanCore.h"
#include "VulkanUniform.h"
#include "VulkanPixelFormat.h"
#include "Rev/Core/Assert.h"
#include "Rev/Render/RenderCmd.h"

namespace Rev
{

struct FVulkanCorePrivate
{
public:
	FVulkanInstance Instance;
	VmaAllocator Allocator = nullptr;


	FVulkanCorePrivate()
	{
		Instance.CreateInstance();
		Instance.CreateSurface();
		Instance.PickPhysicalDevice();
		Instance.CreateLogicalDevice();
		
		VmaAllocatorCreateInfo AllocatorCreateInfo = {};
		AllocatorCreateInfo.physicalDevice = Instance.GetPhysicalDevice();
		AllocatorCreateInfo.device = Instance.GetDevice();
		AllocatorCreateInfo.instance = Instance.GetInstance();
		AllocatorCreateInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
		vmaCreateAllocator(&AllocatorCreateInfo, &Allocator);
	}

	~FVulkanCorePrivate()
	{
		vmaDestroyAllocator(Allocator);
		Instance.Cleanup();
	}
};
static std::unique_ptr<FVulkanCorePrivate> sVulkanCore;


void FVulkanCore::Init()
{
	FVulkanPixelFormat::InitPlatformFormats();
	if(!sVulkanCore)
		sVulkanCore = std::make_unique<FVulkanCorePrivate>();
}

void FVulkanCore::Cleanup()
{
	sVulkanCore.reset();
}

VkInstance FVulkanCore::GetInstance()
{
	return sVulkanCore->Instance.GetInstance();
}

VkSurfaceKHR FVulkanCore::GetSurface()
{
	return sVulkanCore->Instance.GetSurface();
}

VkPhysicalDevice FVulkanCore::GetPhysicalDevice()
{
	return sVulkanCore->Instance.GetPhysicalDevice();
}

VkDevice FVulkanCore::GetDevice()
{
	return sVulkanCore->Instance.GetDevice();
}

VkQueue FVulkanCore::GetQueue(EVulkanQueueKind InKind)
{
	return sVulkanCore->Instance.GetQueue(InKind);
}

uint32 FVulkanCore::GetQueueFamily(EVulkanQueueKind InKind)
{
	return sVulkanCore->Instance.GetQueueFamily(InKind);
}

const FVulkanSurfaceSupport& FVulkanCore::GetSurfaceSupport()
{
	return sVulkanCore->Instance.GetSurfaceSupport();
}

VmaAllocator FVulkanCore::GetAllocator()
{
	return sVulkanCore->Allocator;
}

FVulkanContext* FVulkanCore::GetContext()
{
	REV_CORE_ASSERT(GetRenderAPI() == ERenderAPI::Vulkan);
	return static_cast<FVulkanContext*>(RenderCmd::GetContext());
}

VkCommandBuffer FVulkanCore::GetMainCmdBuffer()
{
	return GetContext()->GetMainCmdBuffer();
}

VkExtent2D FVulkanCore::GetSwapchainExtent()
{
	return GetContext()->GetSwapchain().GetExtent();
}

void FVulkanCore::ImmediateSubmit(std::function<void(VkCommandBuffer)>&& Func)
{
	GetContext()->ImmediateSubmit(std::move(Func));
}

}